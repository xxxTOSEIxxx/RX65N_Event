#include "Task_Button_Global.h"
#include "Event.h"



#define	POLLING_TIME			( 5 )						// ボタン監視間隔(ms)
#define PUSH_TIME				( 50 )						// ボタン押下確定時間(ms)
#define LONG_PUSH_TIME			( 3000 )					// ボタン長押し確定時間(ms)


TASK_BUTTON_GLOBAL_TABLE		g_tButtonGlobal;

//-------------------------------------------------
// ボタン情報定義
//-------------------------------------------------
const TASK_BUTTON_INFO_TABLE g_tButtonInfo[TASK_BUTTON_MAX] =
{
	{{ GPIO_PORT_A_PIN_2, true }, { BUTTON_EVENT_SW_PRESS, 			BUTTON_EVENT_SW_RELEASE, 			BUTTON_EVENT_SW_LONG_PRESS			}},	// 北斗電子評価ボードのスイッチ
	{{ GPIO_PORT_D_PIN_0, true }, { BUTTON_EVENT_COURSE_PRESS, 		BUTTON_EVENT_COURSE_RELEASE, 		BUTTON_EVENT_COURSE_LONG_PRESS		}},	// コース切替スイッチ
	{{ GPIO_PORT_D_PIN_1, true }, { BUTTON_EVENT_START_STOP_PRESS, 	BUTTON_EVENT_START_STOP_RELEASE,	BUTTON_EVENT_START_STOP_LONG_PRESS	}},	// スタート/ストップスイッチ
	{{ GPIO_PORT_D_PIN_2, true }, { BUTTON_EVENT_RESET_PRESS, 		BUTTON_EVENT_RESET_RELEASE, 		BUTTON_EVENT_RESET_LONG_PRESS		}},	// リセットスイッチ
	{{ GPIO_PORT_D_PIN_3, true }, { BUTTON_EVENT_PASS_PRESS, 		BUTTON_EVENT_PASS_RELEASE, 			BUTTON_EVENT_PASS_LONG_PRESS		}}	// バススイッチ
};


//============================================================================================================
// ボタン情報定義に設定されているボタンを使用できるようにする
//============================================================================================================
void InitButton(void)
{
	gpio_err_t				eGpioErr = GPIO_SUCCESS;
	gpio_cmd_t				eGpioCmd = GPIO_CMD_IN_PULL_UP_DISABLE;
	uint16_t				i = 0;


	for ( i = 0 ; i < TASK_BUTTON_MAX ; i++)
	{
		//　状態初期化
		g_tButtonGlobal.tButtonStatus[i].bPush = false;
		g_tButtonGlobal.tButtonStatus[i].Time = 0;
		g_tButtonGlobal.tButtonStatus[i].eEventStatus = TASK_BUTTON_EVENT_STATUS_NOT_SEND;

		// 入力PINとして設定
		R_GPIO_PinDirectionSet(g_tButtonInfo[i].tPin.ePin, GPIO_DIRECTION_INPUT);

		// プルアップ抵抗の有り/無しを設定
		eGpioCmd = ((g_tButtonInfo[i].tPin.bPullup == true) ? GPIO_CMD_IN_PULL_UP_ENABLE : GPIO_CMD_IN_PULL_UP_DISABLE);
		eGpioErr = R_GPIO_PinControl(g_tButtonInfo[i].tPin.ePin, eGpioCmd);
		if (eGpioErr != GPIO_SUCCESS)
		{
			printf("R_GPIO_PinControl Error.\n");
		}
	}
}


//============================================================================================================
// Task_Buttonメイン処理
//============================================================================================================
void Task_Button_Main(void)
{
	gpio_level_t			eGpioLevel = GPIO_LEVEL_LOW;
	uint16_t				i = 0;

	// ボタン情報定義に設定されているボタンを使用できるようにする
	InitButton();

	// Buttonタスク用イベント生成
	CreateEvent(TASK_KIND_BUTTON);

	while(1)
	{
		// ボタンが押されているかを判定
		for ( i = 0 ; i < TASK_BUTTON_MAX ; i++)
		{
			eGpioLevel = R_GPIO_PinRead(g_tButtonInfo[i].tPin.ePin);

			// ボタンが押されている場合
			if ( eGpioLevel == ((g_tButtonInfo[i].tPin.bPullup == true) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH))
			{
				// OFF　→ ON
				if (g_tButtonGlobal.tButtonStatus[i].bPush == false)
				{
					g_tButtonGlobal.tButtonStatus[i].bPush = true;
					g_tButtonGlobal.tButtonStatus[i].Time = 0;
					g_tButtonGlobal.tButtonStatus[i].eEventStatus = TASK_BUTTON_EVENT_STATUS_NOT_SEND;
				}
				else
				{
					// ボタン監視時間(ms)を足しこむ
					g_tButtonGlobal.tButtonStatus[i].Time += POLLING_TIME;

					// ボタン長押し確定時間を超えている場合
					if (g_tButtonGlobal.tButtonStatus[i].Time >= LONG_PUSH_TIME)
					{
						// 「ボタン長押しイベント」を送信していない場合
						if (g_tButtonGlobal.tButtonStatus[i].eEventStatus != TASK_BUTTON_EVENT_STATUS_SEND_LONG_PRESS_EVENT)
						{
							// ボタン長押しイベント送信
							SendEvent(TASK_KIND_APP_MAIN,PRIORITY_KIND_NORMAL, g_tButtonInfo[i].tEvent.LongPressEvent, portMAX_DELAY);
							g_tButtonGlobal.tButtonStatus[i].eEventStatus = TASK_BUTTON_EVENT_STATUS_SEND_LONG_PRESS_EVENT;
//							printf("SEND_LONG_PRESS_EVENT [i:%d]\n", i);
						}
					}
					// ボタン押し確定時間を超えている場合
					else if (g_tButtonGlobal.tButtonStatus[i].Time >= PUSH_TIME)
					{
						// 「イベント未送信」の場合
						if (g_tButtonGlobal.tButtonStatus[i].eEventStatus == TASK_BUTTON_EVENT_STATUS_NOT_SEND)
						{
							// ボタン押下イベント送信
							SendEvent(TASK_KIND_APP_MAIN,PRIORITY_KIND_NORMAL, g_tButtonInfo[i].tEvent.PressEvent, portMAX_DELAY);
							g_tButtonGlobal.tButtonStatus[i].eEventStatus = TASK_BUTTON_EVENT_STATUS_SEND_PRESS_EVENT;
//							printf("SEND_PRESS_EVENT [i:%d]\n", i);
						}
					}
				}
			}
			// ボタンが押されていない場合
			else
			{
				// ON → OFF
				if (g_tButtonGlobal.tButtonStatus[i].bPush == true)
				{
					// 既に「ボタン押下イベント」を送信している場合
					if (g_tButtonGlobal.tButtonStatus[i].eEventStatus == TASK_BUTTON_EVENT_STATUS_SEND_PRESS_EVENT)
					{
						// ボタンリリースイベント送信
						SendEvent(TASK_KIND_APP_MAIN,PRIORITY_KIND_NORMAL, g_tButtonInfo[i].tEvent.ReleaseEvent, portMAX_DELAY);
//						printf("SEND_RELEASE_EVENT [i:%d]\n", i);
					}
					g_tButtonGlobal.tButtonStatus[i].bPush = false;
					g_tButtonGlobal.tButtonStatus[i].Time = 0;
					g_tButtonGlobal.tButtonStatus[i].eEventStatus = TASK_BUTTON_EVENT_STATUS_NOT_SEND;
				}
			}
		}

		vTaskDelay(POLLING_TIME);
	}
}

