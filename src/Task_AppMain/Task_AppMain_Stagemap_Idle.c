#include "Task_AppMain_Global.h"
#include "Task_AppMain_Stagemap_Idle.h"
#include "Task_AppMain_Stagmap.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "LedApi.h"
#include "Event.h"
#include "AudioPlaybackApi.h"

extern TASK_APP_MAIN_GLOBAL_TABLE		g_tAppMain;


//=================================================================================================
// 待受け状態（初期化処理)
//=================================================================================================
void Task_App_Stagemap_Idle_Init(void)
{
//	printf("---< Task_App_Stagemap_Idle_Init >---\n");

	// 北斗電子評価ボードのLEDを点灯させる
	SetLed( LED_KIND_LED, LED_CONTROL_ON);

	g_tAppMain.eAudioData = AUDIOPLAYBACK_AUDIO_DATA_001;
}


//=================================================================================================
// 待受け状態（メイン処理)
//=================================================================================================
void Task_App_Stagemap_Idle_Proc(void)
{
	uint32_t				Event = 0x00000000;
	AUDIOPLAYBACK_REQ_INFO_TABLE	tReqInfo;


//	printf("---< Task_App_Stagemap_Idle_Proc >---\n");

	// 北斗電子評価ボードのスイッチのリリースイベントがくるまで待つ
	ReceiveEvent( TASK_KIND_APP_MAIN, &Event, portMAX_DELAY);
	printf("  ReceiveEvent : %08X\n", Event);
	switch(Event) {
	case BUTTON_EVENT_SW_RELEASE:	// 北斗電子評価ボードのスイッチリリースイベント
		// 状態を「コース選択状態」へ遷移
		//SetStageMap(&g_tAppMain.StageMapHandle, TASK_APP_MAIN_STAGE_MAP_SELECT_COURSE);
		tReqInfo.ePlaybackData = g_tAppMain.eAudioData;
		tReqInfo.eTaskKind = TASK_KIND_APP_MAIN;
		AudioPlayback_Start(&tReqInfo);

		g_tAppMain.eAudioData++;
		if (g_tAppMain.eAudioData >= AUDIOPLAYBACK_AUDIO_DATA_MAX) g_tAppMain.eAudioData = AUDIOPLAYBACK_AUDIO_DATA_001;
		break;

	default:
		// 何もしない
		break;
	}
}


//=================================================================================================
// 待受け状態（終了処理)
//=================================================================================================
void Task_App_Stagemap_Idle_End(void)
{
//	printf("---< Task_App_Stagemap_Idle_End >---\n");
}


