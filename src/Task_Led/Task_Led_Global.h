#ifndef TASK_LED_TASK_LED_GLOBAL_H_
#define TASK_LED_TASK_LED_GLOBAL_H_
#include "LedApi.h"
#include "semphr.h"




//-------------------------------------------------------------------
// LED制御状態構造体
//-------------------------------------------------------------------
typedef struct
{
	LED_CONTROL_ENUM					eControlRequest;	// LED制御種別要求
	LED_CONTROL_ENUM					eControl;			// LED制御種別
	uint8_t								index;				// インデックス
	int32_t								Time;				// 制御時間

} LED_CONTROL_STATUS_TABLE;



//-------------------------------------------------------------------
// LEDタスク用グローバル変数構造体定義
//-------------------------------------------------------------------
typedef struct
{
	QueueHandle_t						MutexHandle;						// Ledタスク用ミューテックスハンドル
	LED_CONTROL_STATUS_TABLE			tControlStatus[LED_KIND_MAX];		// LED制御状態

} TASK_LED_GLOBAL_TABLE;

#endif /* TASK_LED_TASK_LED_GLOBAL_H_ */
