#ifndef EVENT_H_
#define EVENT_H_
#include "Task_def.h"
#include "Event_def.h"
#include "stdbool.h"

//-------------------------------------------------------------------
// プライオリティ種別
//-------------------------------------------------------------------
typedef enum
{
	PRIORITY_KIND_NORMAL = 0,			// 優先度：通常
	PRIORITY_KIND_HIGH,					// 優先度：高
	PRIORITY_KIND_MAX					// === 終端 ===

} PRIORITY_KIND_ENUM;


//=================================================================================================
// イベント初期化処理(※システム側でコールすること)
//=================================================================================================
void InitEvent(void);

//=================================================================================================
// イベント生成
//=================================================================================================
void CreateEvent(TASK_KIND_ENUM eTaskKind);

//============================================================================================================
// イベント送信
//============================================================================================================
bool SendEvent(TASK_KIND_ENUM eTaskKind, PRIORITY_KIND_ENUM ePriorityKind, uint32_t Event, TickType_t xTicksToWait);

//============================================================================================================
// イベント送信(ISR用)
//============================================================================================================
bool SendEventFromISR(TASK_KIND_ENUM eTaskKind, PRIORITY_KIND_ENUM ePriorityKind, uint32_t Event, BaseType_t* pxHigherPriorityTaskWoken);

//============================================================================================================
// イベント受信処理
//============================================================================================================
bool ReceiveEvent(TASK_KIND_ENUM eTaskKind, uint32_t *pEvent, TickType_t xTicksToWait);


#endif /* EVENT_H_ */
