#include "FreeRTOS.h"
#include "Event.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#define QUEUE_LENGTH				( 20 )											// キューの長さ

#define EVENT_GROUP_BIT_SYSTEM		( 0x00000001 )									// イベントグループ用システムBit
#define EVENT_GROUP_BIT_NORMAL		( 0x00000010 )									// イベントグループ用通常bit
#define EVENT_GROUP_WAIT_BIT		( EVENT_GROUP_BIT_SYSTEM | EVENT_GROUP_BIT_NORMAL )


//-------------------------------------------------------------------
// タスク用イベント構造体
//-------------------------------------------------------------------
typedef struct
{
	QueueHandle_t					MutexHandle;									// ミューテックスハンドル
	EventGroupHandle_t				EventGroupHandle;								// イベントグループハンドル
	QueueHandle_t					QueueHandle[PRIORITY_KIND_MAX];					// キューハンドル(※プライオリティ種別分※)

} TASK_EVENT_TABLE;


//-------------------------------------------------------------------
// Event用グローバル変数構造体
//-------------------------------------------------------------------
typedef struct
{
	bool							bInit;

	TASK_EVENT_TABLE				tTaskEvent[TASK_KIND_MAX];						// タスク用イベント

} EVENT_GLOBAL_TABLE;

EVENT_GLOBAL_TABLE					g_tEvent;



//=================================================================================================
// イベント初期化処理(※システム側でコールすること)
//=================================================================================================
void InitEvent(void)
{
	// グローバル変数の初期化
	memset(&g_tEvent, 0x00, sizeof(g_tEvent));
	g_tEvent.bInit = true;
}


//=================================================================================================
// イベント生成
//=================================================================================================
void CreateEvent(TASK_KIND_ENUM eTaskKind)
{
	uint8_t					i = 0;


	if (g_tEvent.bInit != true)
	{
		InitEvent();
	}

	// 既に作成している場合
	if (g_tEvent.tTaskEvent[eTaskKind].MutexHandle != NULL)
	{
		return;
	}

	// ミューテックスハンドル生成
	g_tEvent.tTaskEvent[eTaskKind].MutexHandle = xSemaphoreCreateMutex();

	// イベントグループハンドル生成
	g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle = xEventGroupCreate();

	// キューハンドル生成
	for(i = 0 ; i < PRIORITY_KIND_MAX ; i++)
	{
		g_tEvent.tTaskEvent[eTaskKind].QueueHandle[i] = xQueueCreate(QUEUE_LENGTH, sizeof(uint32_t));
	}
}


//============================================================================================================
// イベント送信
//============================================================================================================
bool SendEvent(TASK_KIND_ENUM eTaskKind, PRIORITY_KIND_ENUM ePriorityKind, uint32_t Event, TickType_t xTicksToWait)
{
	BaseType_t					Result = pdFALSE;
	EventBits_t					EventBit = 0x00000000;


	// ▼----------------------------------▼
	xSemaphoreTake(g_tEvent.tTaskEvent[eTaskKind].MutexHandle, portMAX_DELAY);

	// イベント送信
	Result = xQueueSend(g_tEvent.tTaskEvent[eTaskKind].QueueHandle[ePriorityKind], &Event, xTicksToWait);
	if (Result == pdTRUE)
	{
		// イベントグループbitを設定
		switch( ePriorityKind){
		case PRIORITY_KIND_HIGH:					// 優先度：高
			EventBit = EVENT_GROUP_BIT_SYSTEM;
			break;
		case PRIORITY_KIND_NORMAL:					// 優先度：通常
		default:
			EventBit = EVENT_GROUP_BIT_NORMAL;
			break;
		}
		xEventGroupSetBits(g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle, EventBit);
	}

	xSemaphoreGive(g_tEvent.tTaskEvent[eTaskKind].MutexHandle);
	// ▲----------------------------------▲

	return ((Result == pdTRUE) ? true : false);
}


//============================================================================================================
// イベント送信(ISR用)
//============================================================================================================
bool SendEventFromISR(TASK_KIND_ENUM eTaskKind, PRIORITY_KIND_ENUM ePriorityKind, uint32_t Event, BaseType_t* pxHigherPriorityTaskWoken)
{
	BaseType_t					Result = pdFALSE;
	QueueHandle_t				MutexHandle = NULL;
	EventGroupHandle_t			EventGroupHandle = NULL;
	QueueHandle_t				QueueHandle = NULL;
	EventBits_t					EventBits = 0x00000000;
	BaseType_t					HigherPriorityTaskWoken = pdFALSE;


	// 引数チェック
	if (pxHigherPriorityTaskWoken == NULL)
	{
		return false;
	}
	*pxHigherPriorityTaskWoken = pdFALSE;

	// 指定タスクのミューテックスハンドル/イベントハンドル/キューハンドルのチェック
	MutexHandle = g_tEvent.tTaskEvent[eTaskKind].MutexHandle;
	EventGroupHandle = g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle;
	QueueHandle = g_tEvent.tTaskEvent[eTaskKind].QueueHandle[ePriorityKind];
	if ((MutexHandle == NULL) || (EventGroupHandle == NULL) || (QueueHandle == NULL))
	{
		return false;
	}

	// ▼----------------------------------▼
	HigherPriorityTaskWoken = pdFALSE;
	xSemaphoreTakeFromISR(MutexHandle, &HigherPriorityTaskWoken);
	if (HigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;

	// イベント送信
	HigherPriorityTaskWoken = pdFALSE;
	Result = xQueueSendFromISR(QueueHandle, &Event, &HigherPriorityTaskWoken);
	if (Result == pdTRUE)
	{
		// イベントグループbitを設定
		switch( ePriorityKind){
		case PRIORITY_KIND_HIGH:					// 優先度：高
			EventBits = EVENT_GROUP_BIT_SYSTEM;
			break;
		case PRIORITY_KIND_NORMAL:					// 優先度：通常
		default:
			EventBits = EVENT_GROUP_BIT_NORMAL;
			break;
		}
		HigherPriorityTaskWoken = pdFALSE;
		xEventGroupSetBitsFromISR(EventGroupHandle, EventBits, &HigherPriorityTaskWoken);
		if (HigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;
	}

	HigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(MutexHandle,&HigherPriorityTaskWoken);
	if (HigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;
	// ▲----------------------------------▲

	return ((Result == pdTRUE) ? true : false);
}


//============================================================================================================
// イベント受信処理
//============================================================================================================
bool ReceiveEvent(TASK_KIND_ENUM eTaskKind, uint32_t *pEvent, TickType_t xTicksToWait)
{
	BaseType_t					Result = pdFALSE;
	EventBits_t					EventBits = 0x00000000;


	// 引数チェック
	if (pEvent == NULL)
	{
		return false;
	}
	*pEvent = 0x00000000;

	// イベントグループBitが立つのを待つ
//	printf("eTaskKind:%d , EventGroupHandle:%p\n",eTaskKind,g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle);
	EventBits = xEventGroupWaitBits(g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle, EVENT_GROUP_WAIT_BIT, pdFALSE, pdFALSE, xTicksToWait);
	if (EventBits == 0x00000000)
	{
		*pEvent = SYSTEM_EVENT_TIMEOUT;
	}
	else
	{
		// ▼----------------------------------▼
		xSemaphoreTake(g_tEvent.tTaskEvent[eTaskKind].MutexHandle, portMAX_DELAY);

		// システムイベントを優先的に取得
		if (EventBits & EVENT_GROUP_BIT_SYSTEM)
		{
			// システムイベントを受信
			Result = xQueueReceive(g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_HIGH], pEvent, portMAX_DELAY);

			// システム用のキューに何も入っていない場合は、イベントグループ用システムBitをクリアする
			if (xQueueIsQueueEmptyFromISR(g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_HIGH]) == pdTRUE)
			{
				xEventGroupClearBits(g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle, EVENT_GROUP_BIT_SYSTEM);
			}
		}
		// 通常イベントを取得
		else if (EventBits & EVENT_GROUP_BIT_NORMAL)
		{
			// 通常イベントを受信
			Result = xQueueReceive(g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_NORMAL], pEvent, portMAX_DELAY);

			// 通常用のキューに何も入っていない場合は、イベントグループ用通常Bitをクリアする
			if (xQueueIsQueueEmptyFromISR(g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_NORMAL]) == pdTRUE)
			{
				xEventGroupClearBits(g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle, EVENT_GROUP_BIT_NORMAL);
			}
		}
		// あり得ないけど(馬鹿よけ)
		else
		{
			*pEvent = 0x00000000;
		}

		xSemaphoreGive(g_tEvent.tTaskEvent[eTaskKind].MutexHandle);
		// ▲----------------------------------▲
	}

	return ((Result == pdTRUE) ? true : false);
}


//============================================================================================================
// イベントクリア処理
//============================================================================================================
void ClearEvent(TASK_KIND_ENUM eTaskKind)
{
	QueueHandle_t				MutexHandle = NULL;
	EventGroupHandle_t			EventGroupHandle = NULL;
	QueueHandle_t				High_QueueHandle = NULL;
	QueueHandle_t				Normal_QueueHandle = NULL;


	// 指定タスクのミューテックスハンドル/イベントハンドル/キューハンドルのチェック
	MutexHandle = g_tEvent.tTaskEvent[eTaskKind].MutexHandle;
	EventGroupHandle = g_tEvent.tTaskEvent[eTaskKind].EventGroupHandle;
	High_QueueHandle = g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_HIGH];
	Normal_QueueHandle = g_tEvent.tTaskEvent[eTaskKind].QueueHandle[PRIORITY_KIND_NORMAL];
	if ((MutexHandle == NULL) || (EventGroupHandle == NULL) || (High_QueueHandle == NULL) || (Normal_QueueHandle == NULL))
	{
		return;
	}

	// ▼----------------------------------▼
	xSemaphoreTake(MutexHandle, portMAX_DELAY);

	// 通常用のキューをクリアする
	xQueueReset(Normal_QueueHandle);
	xEventGroupClearBits(EventGroupHandle, EVENT_GROUP_BIT_NORMAL);

	xSemaphoreGive(MutexHandle);
	// ▲----------------------------------▲
}


