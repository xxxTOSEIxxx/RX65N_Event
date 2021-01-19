#include "Task_Led_Global.h"
#include "r_gpio_rx_if.h"

#define	LED_ON							( true )
#define	LED_OFF							( false )
#define LED_CONTORL_INFO_TABLE_COL		( 11 )
#define LED_CONTROL_TIME				( 50 )						// LED制御間隔(ms)

// LEDタスク用グローバル変数
TASK_LED_GLOBAL_TABLE					g_tLed;


//-------------------------------------------------------------------
// LED端子設定情報
//-------------------------------------------------------------------
typedef struct
{
	gpio_port_pin_t						ePin;						// 端子種別
	gpio_cmd_t							eCommand;					// 端子制御コマンド

} PIN_INFO_TABLE;

const PIN_INFO_TABLE 					g_tPinInfo[LED_KIND_MAX] =
{
	{ GPIO_PORT_6_PIN_0, GPIO_CMD_ASSIGN_TO_GPIO },			// コース01_LED
	{ GPIO_PORT_6_PIN_1, GPIO_CMD_ASSIGN_TO_GPIO },			// コース02_LED
	{ GPIO_PORT_6_PIN_2, GPIO_CMD_ASSIGN_TO_GPIO },			// コース03_LED
	{ GPIO_PORT_6_PIN_3, GPIO_CMD_ASSIGN_TO_GPIO },			// コース04_LED
	{ GPIO_PORT_6_PIN_4, GPIO_CMD_ASSIGN_TO_GPIO },			// コース05_LED
	{ GPIO_PORT_6_PIN_6, GPIO_CMD_ASSIGN_TO_GPIO },			// フリッカーランプ01
	{ GPIO_PORT_6_PIN_7, GPIO_CMD_ASSIGN_TO_GPIO },			// フリッカーランプ02
	{ GPIO_PORT_A_PIN_1, GPIO_CMD_ASSIGN_TO_GPIO },			// 北斗電子のLED
};


//-------------------------------------------------------------------
// LED制御情報
//-------------------------------------------------------------------
typedef struct
{
	bool							bOn;			// LED点灯/消灯フラグ(true:点灯 , false:消灯)
	int32_t							Time;			// 制御時間

} LED_CONTORL_INFO_TABLE;

const LED_CONTORL_INFO_TABLE		g_tLedControlInfo[LED_CONTROL_MAX][LED_CONTORL_INFO_TABLE_COL] =
{
	{{ LED_OFF, 60000 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED消灯
	{{ LED_ON , 60000 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点灯
	{{ LED_ON ,   500 }, { LED_OFF,   500 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点滅(0.5秒毎に点灯/消灯を繰返す)
	{{ LED_ON ,   100 }, { LED_OFF,   500 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点滅01
	{{ LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   500 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点滅02
	{{ LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   500 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点滅03
	{{ LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   500 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED点滅04
	{{ LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   100 }, { LED_ON ,   100 }, { LED_OFF,   500 }, { LED_OFF,    -1 }},	// LED点滅05
	{{ LED_ON ,   100 }, { LED_OFF,   100 }, { LED_OFF ,   -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }, { LED_OFF,    -1 }},	// LED_ERR点滅
};



//=================================================================================================
// LEDタスク 初期化処理
//=================================================================================================
void Task_Led_Init(void)
{
//	gpio_err_t					eGipoErr = GPIO_SUCCESS;
	uint16_t					i = 0;


	// ▼----------------------------------▼
	xSemaphoreTake(g_tLed.MutexHandle, portMAX_DELAY);

	for (i = 0 ; i < LED_KIND_MAX ; i++ )
	{
		g_tLed.tControlStatus[i].eControlRequest = LED_CONTROL_OFF;
		g_tLed.tControlStatus[i].eControl = LED_CONTROL_OFF;
		g_tLed.tControlStatus[i].Time = 0;
		g_tLed.tControlStatus[i].index = 0;

		// 端子の報告を設定(OUT固定)
		R_GPIO_PinDirectionSet(g_tPinInfo[i].ePin, GPIO_DIRECTION_OUTPUT);

		// 端子制御設定(「GPIO_CMD_ASSIGN_TO_GPIO」以外なら設定を行う
		if (g_tPinInfo[i].eCommand != GPIO_CMD_ASSIGN_TO_GPIO)
		{
			R_GPIO_PinControl(g_tPinInfo[i].ePin, g_tPinInfo[i].eCommand);
		}

		// 端子レベルを設定(Low固定)
		R_GPIO_PinWrite(g_tPinInfo[i].ePin, GPIO_LEVEL_LOW);
	}

	xSemaphoreGive(g_tLed.MutexHandle);
	// ▲----------------------------------▲
}


//=================================================================================================
// LEDタスク メイン処理
//=================================================================================================
void Task_Led_Main(void)
{
	uint8_t						i = 0;


	// Ledタスク用ミューテックスハンドルを生成
	g_tLed.MutexHandle = xSemaphoreCreateMutex();

	// LEDタスク初期化処理
	Task_Led_Init();

	while(1)
	{
		for( i = 0 ; i < LED_KIND_MAX ; i++ )
		{
			// LED制御種別要求　と　LED制御種別が一致していない場合
			if (g_tLed.tControlStatus[i].eControlRequest != g_tLed.tControlStatus[i].eControl)
			{
				g_tLed.tControlStatus[i].eControl = g_tLed.tControlStatus[i].eControlRequest;
				g_tLed.tControlStatus[i].Time = 0;
				g_tLed.tControlStatus[i].index = 0;

				// LED制御
				R_GPIO_PinWrite(g_tPinInfo[i].ePin, ((g_tLedControlInfo[g_tLed.tControlStatus[i].eControl][g_tLed.tControlStatus[i].index].bOn == true) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW));

			}
			else
			{
				// LED制御時間がLED制御情報の制御時間を超えた場合
				if (g_tLed.tControlStatus[i].Time >= g_tLedControlInfo[g_tLed.tControlStatus[i].eControl][g_tLed.tControlStatus[i].index].Time)
				{
					g_tLed.tControlStatus[i].Time = 0;
					g_tLed.tControlStatus[i].index++;

					// インデックスがオーバーした場合は、インデックスを0に戻す
					if (g_tLed.tControlStatus[i].index > LED_CONTORL_INFO_TABLE_COL)
					{
						g_tLed.tControlStatus[i].index = 0;
					}

					// LED制御時間の設定値が-1の場合も、インデックスを0に戻す
					if (g_tLedControlInfo[g_tLed.tControlStatus[i].eControl][g_tLed.tControlStatus[i].index].Time < 0)
					{
						g_tLed.tControlStatus[i].index = 0;
					}

					// LED制御
					R_GPIO_PinWrite(g_tPinInfo[i].ePin, ((g_tLedControlInfo[g_tLed.tControlStatus[i].eControl][g_tLed.tControlStatus[i].index].bOn == true) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW));
				}

				// 後で足しこむのは面倒なので、先にLED制御時間にLED制御時間間隔を足しこむ
				g_tLed.tControlStatus[i].Time += LED_CONTROL_TIME;
			}
		}

		vTaskDelay(LED_CONTROL_TIME);
	}
}




