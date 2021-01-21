#include "Task_AudioPlayback_Global.h"
#include "Event.h"
#include "Task_AudioPlayback_Main.h"
#include "Task_AudioPlayback_Stagemap.h"


extern TASK_AUDIOPLAYBACK_GLOBAL_TABLE					g_tAudioPlayback;


//=================================================================================================
// 音声再生再生中状態（初期化処理)
//=================================================================================================
void Task_AudioPlayback_Play_Init(void)
{
}


//=================================================================================================
// 音声再生再生中状態（メイン処理)
//=================================================================================================
void Task_AudioPlayback_Play_Proc(void)
{
	bool					bResult = false;
	int8_t					iResult = 0;
	uint32_t				Event = 0x00000000;


	// 要求イベントを受信
	ReceiveEvent(TASK_KIND_AUDIO_PLAYBACK, &Event, 20);
	switch (Event) {
	case SYSTEM_EVENT_TIMEOUT:					// タイムアウト
		//　エンコード処理(※戻り値： 0:音声変換データが一定サイズ保持された , 1：音声変換データが最後まで変換された , -1：異常)
		iResult = Task_AudioPlayback_Encode();
		switch(iResult) {
		case 0:		// 音声変換データが一定サイズ保持された
		case 1:		// 音声変換データが最後まで変換された
			// 音声再生処理継続
			break;

		case -1:	// 異常終了
		default:
			// 音声再生停止処理
			Task_AudioPlayback_Stop();

			// 自分の意志で終了したわけではないので、再生要求タスクへ「音声再生終了イベント」を返す
			SendEvent(g_tAudioPlayback.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_AUDIO_END, portMAX_DELAY);

			// 「停止状態」へ遷移
			SetStageMap(&g_tAudioPlayback.StageMapHandle, TASK_AUDIOPLAYBAKC_STAGEMAP_PLAY);
			break;
		}
		break;

	case AUDIO_PLAYBACK_EVENT_START_REQ:		// 音声再生開始要求イベント
		// 音声再生停止処理
		Task_AudioPlayback_Stop();
		if (g_tAudioPlayback.tReqInfo.eTaskKind != g_tAudioPlayback.eTaskKind)
		{
			// 自分の意志で終了したわけではないので、再生要求タスクへ「音声再生終了イベント」を返す
			SendEvent(g_tAudioPlayback.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_AUDIO_END, portMAX_DELAY);
		}
		// 音声再生開始処理
		bResult = Task_AudioPlayback_Start(g_tAudioPlayback.tReqInfo.eTaskKind, g_tAudioPlayback.tReqInfo.ePlaybackData);
		if (bResult == true)
		{
			// API側に「成功」を返す
			SendEvent(g_tAudioPlayback.tReqInfo.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_SUCCESS, portMAX_DELAY);
		}
		else
		{
			// API側に「エラーイベント」を返す
			SendEvent(g_tAudioPlayback.tReqInfo.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_ERROR, portMAX_DELAY);

			// 「停止状態」へ遷移
			SetStageMap(&g_tAudioPlayback.StageMapHandle, TASK_AUDIOPLAYBAKC_STAGEMAP_PLAY);
		}
		break;

	case AUDIO_PLAYBACK_EVENT_STOP_REQ:			// 音声再生停止要求イベント
		// 音声再生停止処理
		Task_AudioPlayback_Stop();
		if (g_tAudioPlayback.tReqInfo.eTaskKind != g_tAudioPlayback.eTaskKind)
		{
			// 自分の意志で終了したわけではないので、再生要求タスクへ「音声再生終了イベント」を返す
			SendEvent(g_tAudioPlayback.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_AUDIO_END, portMAX_DELAY);
		}
		// 「停止状態」へ遷移
		SetStageMap(&g_tAudioPlayback.StageMapHandle, TASK_AUDIOPLAYBAKC_STAGEMAP_PLAY);
		break;

	default:
		break;
	}
}


//=================================================================================================
// 音声再生再生中状態（終了処理)
//=================================================================================================
void Task_AudioPlayback_Play_End(void)
{
}


