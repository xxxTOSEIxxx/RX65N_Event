#ifndef TASK_LED_LEDAPI_H_
#define TASK_LED_LEDAPI_H_
#include "platform.h"
#include "stdbool.h"


//-------------------------------------------------------------------
// LED種別
//-------------------------------------------------------------------
typedef enum
{
	LED_KIND_COURSE01 = 0,					// コース01_LED
	LED_KIND_COURSE02,						// コース02_LED
	LED_KIND_COURSE03,						// コース03_LED
	LED_KIND_COURSE04,						// コース04_LED
	LED_KIND_COURSE05,						// コース05_LED
	LED_KIND_FLICKER01,						// フリッカーランプ01
	LED_KIND_FLICKER02,						// フリッカーランプ02
	LED_KIND_LED,							// 北斗電子のLED
	LED_KIND_MAX							// === 終端 ===

} LED_KIND_ENUM;


//-------------------------------------------------------------------
// LED制御種別
//-------------------------------------------------------------------
typedef enum
{
	LED_CONTROL_OFF = 0,					// LED消灯
	LED_CONTROL_ON,							// LED点灯
	LED_CONTROL_BLINK,						// LED点滅(0.5秒毎に点灯/消灯を繰返す)
	LED_CONTROL_BLINK_01,					// LED点滅01
	LED_CONTROL_BLINK_02,					// LED点滅02
	LED_CONTROL_BLINK_03,					// LED点滅03
	LED_CONTROL_BLINK_04,					// LED点滅04
	LED_CONTROL_BLINK_05,					// LED点滅05
	LED_CONTROL_BLINK_ERROR,				// LEDエラー点滅
	LED_CONTROL_MAX,						// === 終端 ===

} LED_CONTROL_ENUM;


//-------------------------------------------------------------------
// LEDインターフェース情報構造体
//-------------------------------------------------------------------
typedef struct
{
	LED_KIND_ENUM			eLed;			// LED種別
	LED_CONTROL_ENUM		eControl;		// LED制御種別

} LED_INTERFACE_INFO_TABLE;



//=================================================================================================
// LED設定
//=================================================================================================
bool SetLed(LED_KIND_ENUM eLed, LED_CONTROL_ENUM eControl);


//=================================================================================================
// LED設定（複数同時に設定）
//=================================================================================================
bool SetLedEx(const LED_INTERFACE_INFO_TABLE* ptLedInterfaceInfo, uint8_t InfoNum);


//=================================================================================================
// LED制御情報を取得
//=================================================================================================
LED_CONTROL_ENUM GetLed(LED_KIND_ENUM eLed);


//=================================================================================================
// LED制御情報を取得（複数同時に取得）
//=================================================================================================
bool GetLedEx(LED_INTERFACE_INFO_TABLE* ptLedInterfaceInfo, uint8_t InfoNum);


#endif /* TASK_LED_LEDAPI_H_ */
