#include "platform.h"
#include "r_gpio_rx_if.h"
#include "stdbool.h"



//-------------------------------------------------
// 端子構造体
//-------------------------------------------------
typedef struct
{
	gpio_port_pin_t					ePin;					// 端子種別
	bool 							bPullup;				// プルアップ抵抗フラグ(true：有り / false:無し)

} TASK_BUTTON_PIN_TABLE;

//-------------------------------------------------
// イベント構造体
//-------------------------------------------------
typedef struct
{
	uint32_t						PressEvent;				// ボタン押下イベント
	uint32_t						ReleaseEvent;			// ボタンリリースイベント
	uint32_t						LongPressEvent;			// ボタン長押しイベント

} TASK_BUTTON_EVENT_TABLE;

//-------------------------------------------------
// イベント状態種別
//-------------------------------------------------
typedef enum
{
	TASK_BUTTON_EVENT_STATUS_NOT_SEND = 0,					// イベント未送信
	TASK_BUTTON_EVENT_STATUS_SEND_PRESS_EVENT,				// ボタン押下イベント送信
	TASK_BUTTON_EVENT_STATUS_SEND_RELEASE_EVENT,			// ボタンリリースイベント送信
	TASK_BUTTON_EVENT_STATUS_SEND_LONG_PRESS_EVENT,			// ボタン長押しイベント送信

} TASK_BUTTON_EVENT_STATUS_ENUM;

//-------------------------------------------------
// ボタン情報構造体
//-------------------------------------------------
typedef struct
{
	TASK_BUTTON_PIN_TABLE			tPin;					// 端子情報構造体
	TASK_BUTTON_EVENT_TABLE			tEvent;					// イベント情報構造体

} TASK_BUTTON_INFO_TABLE;


//-------------------------------------------------
// ボタンステータス構造体
//-------------------------------------------------
typedef struct
{
	bool							bPush;					// ボタンが押されているかのフラグ(true:押されている / false:押されていない)
	uint32_t						Time;					// ボタンが押されている時間(ms)
	TASK_BUTTON_EVENT_STATUS_ENUM	eEventStatus;			// イベント状態種別

} TASK_BUTTON_STATUS_TABLE;


//-------------------------------------------------
// ボタン種別 [※ユーザーが定義※]
//-------------------------------------------------
typedef enum
{
	TASK_BUTTON_SW = 0,										// 北斗電子評価ボードの付いているボタン
	TASK_BUTTON_COURSE,										// コース切替スイッチ
	TASK_BUTTON_START_STOP,									// スタート/ストップスイッチ
	TASK_BUTTON_RESET,										// リセットスイッチ
	TASK_BUTTON_PASS,										// パススイッチ
	TASK_BUTTON_MAX											// === 終端 ===

} TASK_BUTTON_ENUM;

//-------------------------------------------------
// グローバル変数構造体
//-------------------------------------------------
typedef struct
{
	TASK_BUTTON_STATUS_TABLE		tButtonStatus[TASK_BUTTON_MAX];		// ボタンステータス

} TASK_BUTTON_GLOBAL_TABLE;




