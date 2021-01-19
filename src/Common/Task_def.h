#ifndef TASK_DEF_H_
#define TASK_DEF_H_

//-------------------------------------------------------------------
// タスク種別
//-------------------------------------------------------------------
typedef enum
{
	TASK_KIND_SYSTEM = 0,						// システムタスク
	TASK_KIND_LED,								// LEDタスク
	TASK_KIND_BUTTON,							// Buttonタスク

	TASK_KIND_APP_MAIN,							// 制御メインタスク
//	TASK_KIND_APP_MACHINE_COM,					// 機械間通信タスク
//	TASK_KIND_APP_PC_COM,						// PC通信タスク

	TASK_KIND_MAX								// === 終端 ===

} TASK_KIND_ENUM;

#endif /* TASK_DEF_H_ */
