#ifndef EVENT_DEF_H_
#define EVENT_DEF_H_
#include "Task_def.h"


#define TASK_EVENT(eTaskKind)						( (((uint32_t)eTaskKind) << 16) )
#define TASK_EVENT_MASK								( 0xFFFF0000 )									// タスクイベントマスク

//-------------------------------------------------------------------
// SYSTEMタスク用イベント
//-------------------------------------------------------------------
#define SYSTEM_EVENT								( TASK_EVENT(TASK_KIND_SYSTEM) )
#define SYSTEM_EVENT_TIMEOUT						( SYSTEM_EVENT | 0x0000FFFF )					// タイムアウトイベント
#define SYSTEM_EVENT_LOW_VOLTAGE					( SYSTEM_EVENT | 0x00000001 )					// 低電圧イベント
#define SYSTEM_EVENT_NORMAL_VOLTAGE					( SYSTEM_EVENT | 0x00000002 )					// 電圧復帰イベント

//-------------------------------------------------------------------
// LEDタスク用イベント
//-------------------------------------------------------------------
#define LED_EVENT									( TASK_EVENT(TASK_KIND_LED) )

//-------------------------------------------------------------------
// Buttonタスク用イベント
//-------------------------------------------------------------------
#define BUTTON_EVENT								( TASK_EVENT(TASK_KIND_BUTTON) )
#define BUTTON_EVENT_PRESS							( 0x00001000 )									// 押下イベント
#define BUTTON_EVENT_RELEASE						( 0x00002000 )									// 押下イベント
#define BUTTON_EVENT_LONG_PRESS						( 0x00003000 )									// 長押しイベント

#define BUTTON_EVENT_SW								( 0x00000000 )									// 北斗電子評価ボードのスイッチ
#define BUTTON_EVENT_COURSE							( 0x00000001 )									// コース切替スイッチ
#define BUTTON_EVENT_START_STOP						( 0x00000002 )									// スタート/ストップスイッチ
#define BUTTON_EVENT_RESET							( 0x00000003 )									// リセットスイッチ
#define BUTTON_EVENT_PASS							( 0x00000004 )									// パススイッチ

#define BUTTON_EVENT_SW_PRESS						( BUTTON_EVENT | BUTTON_EVENT_SW | BUTTON_EVENT_PRESS )						// 北斗電子評価ボードのスイッチ_押下イベント
#define BUTTON_EVENT_SW_RELEASE						( BUTTON_EVENT | BUTTON_EVENT_SW | BUTTON_EVENT_RELEASE )					// 北斗電子評価ボードのスイッチ_リリースイベント
#define BUTTON_EVENT_SW_LONG_PRESS					( BUTTON_EVENT | BUTTON_EVENT_SW | BUTTON_EVENT_LONG_PRESS )				// 北斗電子評価ボードのスイッチ_長押しイベント
#define BUTTON_EVENT_COURSE_PRESS					( BUTTON_EVENT | BUTTON_EVENT_COURSE | BUTTON_EVENT_PRESS )					// コース切替スイッチ_押下イベント
#define BUTTON_EVENT_COURSE_RELEASE					( BUTTON_EVENT | BUTTON_EVENT_COURSE | BUTTON_EVENT_RELEASE )				// コース切替スイッチ_リリースイベント
#define BUTTON_EVENT_COURSE_LONG_PRESS				( BUTTON_EVENT | BUTTON_EVENT_COURSE | BUTTON_EVENT_LONG_PRESS )			// コース切替スイッチ_長押しイベント
#define BUTTON_EVENT_START_STOP_PRESS				( BUTTON_EVENT | BUTTON_EVENT_START_STOP | BUTTON_EVENT_PRESS )				// スタート/ストップスイッチ_押下イベント
#define BUTTON_EVENT_START_STOP_RELEASE				( BUTTON_EVENT | BUTTON_EVENT_START_STOP | BUTTON_EVENT_RELEASE )			// スタート/ストップスイッチ_リリースイベント
#define BUTTON_EVENT_START_STOP_LONG_PRESS			( BUTTON_EVENT | BUTTON_EVENT_START_STOP | BUTTON_EVENT_LONG_PRESS )		// スタート/ストップスイッチ_長押しイベント
#define BUTTON_EVENT_RESET_PRESS					( BUTTON_EVENT | BUTTON_EVENT_RESET | BUTTON_EVENT_PRESS )					// リセットスイッチ_押下イベント
#define BUTTON_EVENT_RESET_RELEASE					( BUTTON_EVENT | BUTTON_EVENT_RESET | BUTTON_EVENT_RELEASE )				// リセットスイッチ_リリースイベント
#define BUTTON_EVENT_RESET_LONG_PRESS				( BUTTON_EVENT | BUTTON_EVENT_RESET | BUTTON_EVENT_LONG_PRESS )				// リセットスイッチ_長押しイベント
#define BUTTON_EVENT_PASS_PRESS						( BUTTON_EVENT | BUTTON_EVENT_PASS | BUTTON_EVENT_PRESS )					// パススイッチ_押下イベント
#define BUTTON_EVENT_PASS_RELEASE					( BUTTON_EVENT | BUTTON_EVENT_PASS | BUTTON_EVENT_RELEASE )					// パススイッチ_リリースイベント
#define BUTTON_EVENT_PASS_LONG_PRESS				( BUTTON_EVENT | BUTTON_EVENT_PASS | BUTTON_EVENT_LONG_PRESS )				// パススイッチ_長押しイベント


//-------------------------------------------------------------------
// AudioPlaybackタスク用イベント
//-------------------------------------------------------------------
#define AUDIO_PLAYBACK_EVENT						( TASK_EVENT(TASK_KIND_AUDIO_PLAYBACK) )
#define AUDIO_PLAYBACK_EVENT_START_REQ				( AUDIO_PLAYBACK_EVENT | 0x00000001 )										// 音声再生開始要求イベント
#define AUDIO_PLAYBACK_EVENT_STOP_REQ				( AUDIO_PLAYBACK_EVENT | 0x00000002 )										// 音声再生停止要求イベント

#define AUDIO_PLAYBACK_EVENT_SUCCESS				( AUDIO_PLAYBACK_EVENT | 0x00000011 )										// 成功イベント
#define AUDIO_PLAYBACK_EVENT_ERROR					( AUDIO_PLAYBACK_EVENT | 0x00000012 )										// 異常イベント
#define AUDIO_PLAYBACK_EVENT_AUDIO_END				( AUDIO_PLAYBACK_EVENT | 0x00000013 )										// 音声再生終了イベント



//-------------------------------------------------------------------
// MainAppタスク用イベント
//-------------------------------------------------------------------
#define APP_MAIN_EVENT								( TASK_EVENT(TASK_KIND_APP_MAIN) )

#define APP_MAIN_EVENT_TIMEOUT						( APP_MAIN_EVENT | 0x00000001 )


#endif /* EVENT_DEF_H_ */
