#include "Timer.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define	TIMER_INFO_NUM_MAX					( 10 )
#define TIMER_FREQ							( 100 )

//-------------------------------------------------
// 呼出し種別
//-------------------------------------------------
typedef enum
{
	TIMER_FROM_NORMAL = 0,										// 通常からコール
	TIMER_FROM_ISR,												// ISRからコール

} TIMER_FROM_ENUM;


//-------------------------------------------------
// タイマー情報構造体
//-------------------------------------------------
typedef struct
{
	TimerHandle_t				TimerHandle;					// タイマーハンドル
	TIMER_KIND_ENUM				eTimerKind;						// タイマー種別
	TickType_t 					TimerPeriod;					// タイマー期間(ms)
	TASK_KIND_ENUM				eTaskKind;						// タイマー満了時のイベント送信先タスク
	uint32_t					Event;							// タイマー満了時の送信イベント

} TIMER_INFO_TABLE;


//-------------------------------------------------
// タイマーグローバル変数構造体
//-------------------------------------------------
typedef struct
{
	bool						bInit;
	QueueHandle_t				MutexHandle;					// ミューテックスハンドル

	uint8_t						UseNum;							// タイマー使用件数
	TIMER_INFO_TABLE			tTimerInfo[TIMER_INFO_NUM_MAX];	// タイマー情報

} TIMER_GLOBAL_TABLE;


TIMER_GLOBAL_TABLE				g_tTimer;


//=================================================================================================
// タイマー初期化処理(※システム側でコールすること)
//=================================================================================================
void TimerInit(void)
{
	memset(&g_tTimer, 0x00, sizeof(g_tTimer));

	g_tTimer.bInit = true;

	// ミューテックスハンドル生成
	g_tTimer.MutexHandle = xSemaphoreCreateMutex();
}


//=================================================================================================
// タイマーコールバック(内部関数)
//=================================================================================================
static void timer_callback(TimerHandle_t xTimer)
{
	BaseType_t						Result = pdFAIL;
	TIMER_INFO_TABLE*				ptTimerInfo = NULL;


	configASSERT( xTimer );

	// 識別子を取得
	ptTimerInfo = (TIMER_INFO_TABLE*)pvTimerGetTimerID(xTimer);
	configASSERT( ptTimerInfo );

	// ▼----------------------------------▼
	xSemaphoreTake(g_tTimer.MutexHandle, portMAX_DELAY);

	// イベント送信
	SendEvent(ptTimerInfo->eTaskKind, PRIORITY_KIND_NORMAL, ptTimerInfo->Event, portMAX_DELAY);

	// ワンショットタイマーの場合、タイマー設定情報を削除する
	if (ptTimerInfo->eTaskKind == TIMER_KIND_ONE_SHOT)
	{
		Result = xTimerDelete(ptTimerInfo->TimerHandle, portMAX_DELAY);
		if (Result == pdFAIL)
		{
			printf("xTimerDelete Error.\n");
		}
		memset(ptTimerInfo, 0x00, sizeof(TIMER_INFO_TABLE));

		g_tTimer.UseNum--;
	}

	xSemaphoreGive(g_tTimer.MutexHandle);
	// ▲----------------------------------▲
}


//=================================================================================================
// タイマー設定(内部関数)
//=================================================================================================
static TimerHandle_t set_timer(TIMER_SET_INFO_TABLE* ptTimerSetInfo, TIMER_FROM_ENUM eFrom, BaseType_t* pxHigherPriorityTaskWoken)
{
	BaseType_t			Result = pdFAIL;
	uint8_t				index = 0;


	// 引数チェック
	configASSERT( ptTimerSetInfo );
	configASSERT( pxHigherPriorityTaskWoken );
	*pxHigherPriorityTaskWoken = pdFALSE;

	// 登録件数をチェック
	if (g_tTimer.UseNum >= TIMER_INFO_NUM_MAX)
	{
		return NULL;
	}

	// 登録可能な場所を探す
	for( index = 0 ; index < TIMER_INFO_NUM_MAX ; index++)
	{
		if (g_tTimer.tTimerInfo[index].eTimerKind == TIMER_KIND_NOT_USE)
		{
			break;
		}
	}
	// 馬鹿除け
	if (index >= TIMER_INFO_NUM_MAX)
	{
		return NULL;
	}

	// タイマーハンドルを取得
	g_tTimer.tTimerInfo[index].TimerHandle = xTimerCreate( "Timer",							// タイマー名
			ptTimerSetInfo->TimerPeriod,													// タイマー期間(ms)
			((ptTimerSetInfo->eTimerKind == TIMER_KIND_ONE_SHOT) ? pdFALSE : pdTRUE),		//
			(void *)&g_tTimer.tTimerInfo[index],											// 識別ID
			timer_callback);																// コールバック関数
	if (g_tTimer.tTimerInfo[index].TimerHandle == NULL)
	{
		return NULL;
	}

	// タイマー開始
	if (eFrom == TIMER_FROM_ISR)
	{
		Result = xTimerStartFromISR(g_tTimer.tTimerInfo[index].TimerHandle, pxHigherPriorityTaskWoken);
	}
	else
	{
		Result = xTimerStart(g_tTimer.tTimerInfo[index].TimerHandle, 100);
	}
	if (Result == pdFAIL)
	{
		xTimerDelete(g_tTimer.tTimerInfo[index].TimerHandle, portMAX_DELAY);
		memset(&g_tTimer.tTimerInfo[index], 0x00, sizeof(TIMER_INFO_TABLE));
		return NULL;
	}

	// タイマー登録
	g_tTimer.tTimerInfo[index].eTimerKind = ptTimerSetInfo->eTimerKind;
	g_tTimer.tTimerInfo[index].TimerPeriod = ptTimerSetInfo->TimerPeriod;
	g_tTimer.tTimerInfo[index].eTaskKind = ptTimerSetInfo->eTaskKind;
	g_tTimer.tTimerInfo[index].Event = ptTimerSetInfo->Event;

	g_tTimer.UseNum++;

	return g_tTimer.tTimerInfo[index].TimerHandle;
}


//=================================================================================================
// タイマー設定
//=================================================================================================
TimerHandle_t SetTimer(TIMER_SET_INFO_TABLE* ptTimerSetInfo)
{
	TimerHandle_t		TimerHandle = NULL;
	BaseType_t 			xHigherPriorityTaskWoken = pdFALSE;


	// 引数チェック
	configASSERT( ptTimerSetInfo );

	if (g_tTimer.bInit != true)
	{
		TimerInit();
	}

	// ▼----------------------------------▼
	xSemaphoreTake(g_tTimer.MutexHandle, portMAX_DELAY);

	// タイマー設定
	TimerHandle = set_timer(ptTimerSetInfo, TIMER_FROM_NORMAL, &xHigherPriorityTaskWoken);

	xSemaphoreGive(g_tTimer.MutexHandle);
	// ▲----------------------------------▲

	return TimerHandle;
}


//=================================================================================================
// タイマー設定(ISR用)
//=================================================================================================
TimerHandle_t SetTimerFromISR(TIMER_SET_INFO_TABLE* ptTimerSetInfo, BaseType_t *pxHigherPriorityTaskWoken)
{
	TimerHandle_t		TimerHandle = NULL;
	BaseType_t 			xHigherPriorityTaskWoken = pdFALSE;


	// 引数チェック
	configASSERT( ptTimerSetInfo );
	configASSERT( pxHigherPriorityTaskWoken );
	*pxHigherPriorityTaskWoken = pdFALSE;

	if (g_tTimer.bInit != true)
	{
		TimerInit();
	}

	// ▼----------------------------------▼
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreTakeFromISR(g_tTimer.MutexHandle,&xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;

	// タイマー設定
	xHigherPriorityTaskWoken = pdFALSE;
	TimerHandle = set_timer(ptTimerSetInfo,TIMER_FROM_ISR, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;

	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(g_tTimer.MutexHandle, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;
	// ▲----------------------------------▲

	return TimerHandle;
}


//=================================================================================================
// タイマー削除(内部関数)
//=================================================================================================
static void delete_timer(TimerHandle_t TimerHandle, TIMER_FROM_ENUM eFrom, BaseType_t* pxHigherPriorityTaskWoken)
{
	BaseType_t				Result = pdFAIL;
	TIMER_INFO_TABLE*		ptTimerInfo = NULL;


	// 引数チェック
	configASSERT( TimerHandle );
	configASSERT(pxHigherPriorityTaskWoken);
	*pxHigherPriorityTaskWoken = pdFALSE;

	// 識別子を取得
	ptTimerInfo = (TIMER_INFO_TABLE*)pvTimerGetTimerID(TimerHandle);
	configASSERT( ptTimerInfo );

	// タイマーハンドルがNULL以外の場合
	if (ptTimerInfo->TimerHandle != NULL)
	{
		// タイマー停止
		if (eFrom == TIMER_FROM_ISR)
		{
			Result = xTimerStopFromISR(ptTimerInfo->TimerHandle, pxHigherPriorityTaskWoken);
			if (Result == pdFAIL)
			{
				printf("xTimerStopFromISR Error.\n");
			}
		}
		else
		{
			Result = xTimerStop(ptTimerInfo->TimerHandle, 100);
			if (Result == pdFAIL)
			{
				printf("xTimerStop Error.\n");
			}
		}
		xTimerDelete(ptTimerInfo->TimerHandle, portMAX_DELAY);
		memset(ptTimerInfo, 0x00, sizeof(TIMER_INFO_TABLE));

		g_tTimer.UseNum--;
	}
}


//=================================================================================================
// タイマー削除
//=================================================================================================
void DeleteTimer(TimerHandle_t TimerHandle)
{
	BaseType_t 			xHigherPriorityTaskWoken = pdFALSE;


	// 引数チェック
	configASSERT( TimerHandle );

	// ▼----------------------------------▼
	xSemaphoreTake(g_tTimer.MutexHandle, portMAX_DELAY);

	// タイマー削除
	delete_timer(TimerHandle, TIMER_FROM_NORMAL, &xHigherPriorityTaskWoken);

	xSemaphoreGive(g_tTimer.MutexHandle);
	// ▲----------------------------------▲
}


//=================================================================================================
// タイマー削除(ISR用)
//=================================================================================================
void DeleteTimerISR(TimerHandle_t TimerHandle, BaseType_t *pxHigherPriorityTaskWoken)
{
	BaseType_t 			xHigherPriorityTaskWoken = pdFALSE;


	// 引数チェック
	configASSERT( TimerHandle );
	configASSERT( pxHigherPriorityTaskWoken );
	*pxHigherPriorityTaskWoken = pdFALSE;

	// ▼----------------------------------▼
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreTakeFromISR(g_tTimer.MutexHandle,&xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;

	// タイマー削除
	delete_timer(TimerHandle, TIMER_FROM_ISR, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;

	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(g_tTimer.MutexHandle, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) *pxHigherPriorityTaskWoken = pdTRUE;
	// ▲----------------------------------▲
}



