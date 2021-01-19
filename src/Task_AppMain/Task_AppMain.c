#include "Task_AppMain_Global.h"
#include "Task_AppMain_Stagmap.h"
#include "Event.h"



TASK_APP_MAIN_GLOBAL_TABLE		g_tAppMain;



//=================================================================================================
// AppMainタスク メイン処理
//=================================================================================================
void Task_AppMain_Main(void)
{
	// AppMain用イベント生成
	CreateEvent(TASK_KIND_APP_MAIN);

	// AppMain用ステージマップ初期化（最初の遷移先を「待受け状態」とする）
	InitStageMap(&g_tAppMain.StageMapHandle, g_tAppMainStagemap);
	SetStageMap(&g_tAppMain.StageMapHandle, TASK_APP_MAIN_STAGE_MAP_IDLE);

	// ループ処理
	while( 1 )
	{
		// ステージマップ処理
		StageMapProc(&g_tAppMain.StageMapHandle);
	}
}




