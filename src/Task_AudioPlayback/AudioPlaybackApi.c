#include "AudioPlaybackApi.h"
#include "Task_AudioPlayback_Global.h"
#include "platform.h"
#include "semphr.h"
#include "Event.h"



extern TASK_AUDIOPLAYBACK_GLOBAL_TABLE					g_tAudioPlayback;


//=================================================================================================
// 音声再生開始要求
//=================================================================================================
bool AudioPlayback_Start(AUDIOPLAYBACK_REQ_INFO_TABLE* ptReqInfo)
{
	bool				bResult = false;
	uint32_t			Event = 0x00000000;


	// 引数チェック
	configASSERT( ptReqInfo );

	// ▼----------------------------------▼
	xSemaphoreTake(g_tAudioPlayback.MutexHandle, portMAX_DELAY);

	// リクエスト情報をセット
	g_tAudioPlayback.tReqInfo = *ptReqInfo;

	// AuidoPlaybackタスクへ「音声再生開始イベント」を送信する
	bResult = SendEvent(TASK_KIND_AUDIO_PLAYBACK, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_START_REQ, portMAX_DELAY);
	if (bResult == true)
	{
		// AuidoPlaybackタスクからの結果待ち
		while( (Event != AUDIO_PLAYBACK_EVENT_SUCCESS) && (Event != AUDIO_PLAYBACK_EVENT_ERROR) )
		{
			bResult = ReceiveEvent(ptReqInfo->eTaskKind, &Event, portMAX_DELAY);
			if (bResult == true)
			{
				bResult = ((Event == AUDIO_PLAYBACK_EVENT_SUCCESS) ? true : false);
			}
			else
			{
				break;
			}
		}
	}

	xSemaphoreGive(g_tAudioPlayback.MutexHandle);
	// ▲----------------------------------▲

	return bResult;
}


//=================================================================================================
// 音声再生停止要求
//=================================================================================================
bool AudioPlayback_Stop(TASK_KIND_ENUM eTaskKind)
{
	bool							bResult = false;
	uint32_t						Event = 0x00000000;


	// ▼----------------------------------▼
	xSemaphoreTake(g_tAudioPlayback.MutexHandle, portMAX_DELAY);

	// リクエスト情報をセット
	g_tAudioPlayback.tReqInfo.eTaskKind = eTaskKind;

	// AuidoPlaybackタスクへ「音声再生停止イベント」を送信する
	bResult = SendEvent(TASK_KIND_AUDIO_PLAYBACK, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_STOP_REQ, portMAX_DELAY);
	if (bResult == true)
	{
		// AuidoPlaybackタスクからの結果待ち
		while( (Event != AUDIO_PLAYBACK_EVENT_SUCCESS) && (Event != AUDIO_PLAYBACK_EVENT_ERROR) )
		{
			bResult = ReceiveEvent(eTaskKind, &Event, portMAX_DELAY);
			if (bResult == true)
			{
				bResult = ((Event == AUDIO_PLAYBACK_EVENT_SUCCESS) ? true : false);
			}
			else
			{
				break;
			}
		}
	}

	xSemaphoreGive(g_tAudioPlayback.MutexHandle);
	// ▲----------------------------------▲

	return bResult;
}







