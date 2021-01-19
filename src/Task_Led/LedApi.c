#include "LedApi.h"
#include "Task_Led_Global.h"

// LEDタスク用グローバル変数
extern TASK_LED_GLOBAL_TABLE				g_tLed;


//=================================================================================================
// LED設定
//=================================================================================================
bool SetLed(LED_KIND_ENUM eLed, LED_CONTROL_ENUM eControl)
{
	// ▼----------------------------------▼
	xSemaphoreTake(g_tLed.MutexHandle, portMAX_DELAY);

	g_tLed.tControlStatus[eLed].eControlRequest = eControl;

	xSemaphoreGive(g_tLed.MutexHandle);
	// ▲----------------------------------▲

	return true;
}


//=================================================================================================
// LED設定（複数同時に設定）
//=================================================================================================
bool SetLedEx(const LED_INTERFACE_INFO_TABLE* ptLedInterfaceInfo, uint8_t InfoNum)
{
	uint8_t				i = 0;


	// 引数チェック
	if (ptLedInterfaceInfo == NULL)
	{
		return false;
	}

	if (InfoNum > LED_KIND_MAX)
	{
		return false;
	}

	 // ▼----------------------------------▼
	 xSemaphoreTake(g_tLed.MutexHandle, portMAX_DELAY);

	 for(i = 0 ; i < InfoNum ; i++)
	 {
		 g_tLed.tControlStatus[(ptLedInterfaceInfo[i].eLed)].eControlRequest = ptLedInterfaceInfo[i].eControl;
	 }

	xSemaphoreGive(g_tLed.MutexHandle);
	// ▲----------------------------------▲

	return true;
}


//=================================================================================================
// LED制御情報を取得
//=================================================================================================
LED_CONTROL_ENUM GetLed(LED_KIND_ENUM eLed)
{
	 LED_CONTROL_ENUM 		eControl = LED_CONTROL_OFF;

	// ▼----------------------------------▼
	xSemaphoreTake(g_tLed.MutexHandle, portMAX_DELAY);

	eControl = g_tLed.tControlStatus[eLed].eControl;

	xSemaphoreGive(g_tLed.MutexHandle);
	// ▲----------------------------------▲

	return eControl;
}


//=================================================================================================
// LED制御情報を取得（複数同時に取得）
//=================================================================================================
bool GetLedEx(LED_INTERFACE_INFO_TABLE* ptLedInterfaceInfo, uint8_t InfoNum)
{
	uint8_t				i = 0;


	// 引数チェック
	if (ptLedInterfaceInfo == NULL)
	{
		return false;
	}

	if (InfoNum > LED_KIND_MAX)
	{
		return false;
	}

	 // ▼----------------------------------▼
	 xSemaphoreTake(g_tLed.MutexHandle, portMAX_DELAY);

	 for(i = 0 ; i < InfoNum ; i++)
	 {
		 ptLedInterfaceInfo[i].eControl = g_tLed.tControlStatus[(ptLedInterfaceInfo[i].eLed)].eControl;
	 }

	xSemaphoreGive(g_tLed.MutexHandle);
	// ▲----------------------------------▲

	return true;
}

