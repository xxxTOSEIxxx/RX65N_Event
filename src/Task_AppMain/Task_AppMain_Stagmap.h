#ifndef TASK_APPMAIN_STAGMAP_H_
#define TASK_APPMAIN_STAGMAP_H_
#include "Task_AppMain_Stagemap_Idle.h"
#include "Task_AppMain_Stagemap_SelectCourse.h"
#include "Task_AppMain_Stagemap_MachineOperation.h"
#include "StageMap.h"


// AppMainタスクステージマップ種別
typedef enum
{
	TASK_APP_MAIN_STAGE_MAP_IDLE = 0,						// 待受け状態
	TASK_APP_MAIN_STAGE_MAP_SELECT_COURSE,					// コース選択状態
	TASK_APP_MAIN_STAGE_MAP_MACHINE_OPERATION,				// 稼働状態
	TASK_APP_MAIN_STAGE_MAP_MAX								// === 終端 ===

} TASK_APP_MAIN_STAGE_MAP_ENUM;


// AppMainタスクステージマップ定義
static const STAGE_MAP_TABLE g_tAppMainStagemap[TASK_APP_MAIN_STAGE_MAP_MAX] =
{
	{ Task_App_Stagemap_Idle_Init,				Task_App_Stagemap_Idle_Proc,				Task_App_Stagemap_Idle_End 					},		// 待受け状態
	{ Task_App_Stagemap_SelectCourse_Init,		Task_App_Stagemap_SelectCourse_Proc,		Task_App_Stagemap_SelectCourse_End 			},		// コース選択状態
	{ Task_App_Stagemap_MachineOperation_Init,	Task_App_Stagemap_MachineOperation_Proc,	Task_App_Stagemap_MachineOperation_End 		},		// 稼働状態
};


#endif /* TASK_APPMAIN_STAGMAP_H_ */


