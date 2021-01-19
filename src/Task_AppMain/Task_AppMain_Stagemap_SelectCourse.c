#include "Task_AppMain_Stagemap_SelectCourse.h"
#include "Task_AppMain_Stagmap.h"
#include "Task_AppMain_Global.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "LedApi.h"
#include "Event.h"
#include "Timer.h"

extern TASK_APP_MAIN_GLOBAL_TABLE		g_tAppMain;


//=================================================================================================
// コース選択状態（初期化処理)
//=================================================================================================
void Task_App_Stagemap_SelectCourse_Init(void)
{
	TIMER_SET_INFO_TABLE				tTimerSetInfo;


	printf("---< Task_App_Stagemap_SelectCourse_Init >---\n");

	// 北斗電子評価ボードのLEDを点滅させる
	SetLed( LED_KIND_LED, LED_CONTROL_BLINK);

	// 待受けに戻るタイマーイベントを発行する
	memset(&tTimerSetInfo,0x00,sizeof(tTimerSetInfo));
	tTimerSetInfo.eTaskKind = TASK_KIND_APP_MAIN;
	tTimerSetInfo.Event = APP_MAIN_EVENT_TIMEOUT;
	tTimerSetInfo.TimerPeriod = (30 * 1000);
	tTimerSetInfo.eTimerKind = TIMER_KIND_ONE_SHOT;
	g_tAppMain.TimerHandle = SetTimer(&tTimerSetInfo);
	if (g_tAppMain.TimerHandle == NULL)
	{
		printf("SetTimer Error.\n");
	}
}


//=================================================================================================
// コース選択状態（メイン処理)
//=================================================================================================
void Task_App_Stagemap_SelectCourse_Proc(void)
{
	uint32_t				Event = 0x00000000;


	printf("---< Task_App_Stagemap_SelectCourse_Proc >---\n");

	// 北斗電子評価ボードのスイッチが長押しイベント or タイムアウトイベント(30秒)が来るまで待つ
	ReceiveEvent( TASK_KIND_APP_MAIN, &Event, portMAX_DELAY);
	printf("  ReceiveEvent : %08X\n", Event);
	switch(Event) {
	case BUTTON_EVENT_SW_LONG_PRESS	:	// 北斗電子評価ボードの長押しイベント
		// 状態を「稼働状態」へ遷移
		SetStageMap(&g_tAppMain.StageMapHandle, TASK_APP_MAIN_STAGE_MAP_MACHINE_OPERATION);
		break;

	case APP_MAIN_EVENT_TIMEOUT:		// タイムアウトタイマーイベント
		// 状態を「待受け状態」へ遷移
		SetStageMap(&g_tAppMain.StageMapHandle, TASK_APP_MAIN_STAGE_MAP_IDLE);
		break;

	default:
		// 何もしない
		break;
	}
}


//=================================================================================================
// コース選択状態（終了処理)
//=================================================================================================
void Task_App_Stagemap_SelectCourse_End(void)
{
	printf("---< Task_App_Stagemap_SelectCourse_End >---\n");

	// タイマー削除
	if (g_tAppMain.TimerHandle != NULL)
	{
		DeleteTimer(g_tAppMain.TimerHandle);
		g_tAppMain.TimerHandle = NULL;
	}



}


