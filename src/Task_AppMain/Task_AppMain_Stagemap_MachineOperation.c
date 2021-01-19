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
// 稼働状態（初期化処理)
//=================================================================================================
void Task_App_Stagemap_MachineOperation_Init(void)
{
	TIMER_SET_INFO_TABLE				tTimerSetInfo;


	printf("---< Task_App_Stagemap_MachineOperation_Init >---\n");

	// 北斗電子評価ボードのLEDを高速点滅させる
	SetLed( LED_KIND_LED, LED_CONTROL_BLINK_ERROR);

	// 待受けに戻るタイマーイベントを発行する
	memset(&tTimerSetInfo,0x00,sizeof(tTimerSetInfo));
	tTimerSetInfo.eTaskKind = TASK_KIND_APP_MAIN;
	tTimerSetInfo.Event = APP_MAIN_EVENT_TIMEOUT;
	tTimerSetInfo.TimerPeriod = (10 * 1000);
	tTimerSetInfo.eTimerKind = TIMER_KIND_ONE_SHOT;
	g_tAppMain.TimerHandle = SetTimer(&tTimerSetInfo);
	if (g_tAppMain.TimerHandle == NULL)
	{
		printf("SetTimer Error.\n");
	}
}


//=================================================================================================
// 稼働状態（メイン処理)
//=================================================================================================
void Task_App_Stagemap_MachineOperation_Proc(void)
{
	uint32_t				Event = 0x00000000;


	printf("---< Task_App_Stagemap_MachineOperation_Proc >---\n");

	// タイムアウトイベント(10秒)が来るまで待つ
	ReceiveEvent( TASK_KIND_APP_MAIN, &Event, portMAX_DELAY);
	printf("  ReceiveEvent : %08X\n", Event);
	switch(Event) {
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
// 稼働状態（終了処理)
//=================================================================================================
void Task_App_Stagemap_MachineOperation_End(void)
{
	printf("---< Task_App_Stagemap_MachineOperation_End >---\n");

	// タイマー削除
	if (g_tAppMain.TimerHandle != NULL)
	{
		DeleteTimer(g_tAppMain.TimerHandle);
		g_tAppMain.TimerHandle = NULL;
	}
}


