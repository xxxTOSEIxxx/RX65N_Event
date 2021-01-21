/*
 * Task_AudioPlayback_Stagemap_Stop.c
 *
 *  Created on: 2021/01/19
 *      Author: 99990045
 */
#include "Task_AudioPlayback_Global.h"
#include "Event.h"
#include "Task_AudioPlayback_Main.h"
#include "Task_AudioPlayback_Stagemap.h"


extern TASK_AUDIOPLAYBACK_GLOBAL_TABLE					g_tAudioPlayback;


//=================================================================================================
// 音声再生停止状態（初期化処理)
//=================================================================================================
void Task_AudioPlayback_Stop_Init(void)
{
	g_tAudioPlayback.eStatus = AUDIOPLAYBACK_STATUS_STOP;
//	g_tAudioPlayback.eTaskKind = eKindTask;
	g_tAudioPlayback.eAudioData = AUDIOPLAYBACK_AUDIO_DATA_MAX;
	g_tAudioPlayback.pAudioData = NULL;
	g_tAudioPlayback.AudioDataSize = 0;
	g_tAudioPlayback.DecodeIndex = 0;
	g_tAudioPlayback.OutputIndex = 0;
}

//=================================================================================================
// 音声再生停止状態（メイン処理)
//=================================================================================================
void Task_AudioPlayback_Stop_Proc(void)
{
	bool					bResult = false;
	uint32_t				Event = 0x00000000;


	// 要求イベントを受信
	ReceiveEvent(TASK_KIND_AUDIO_PLAYBACK, &Event, portMAX_DELAY);
	switch (Event) {
	case AUDIO_PLAYBACK_EVENT_START_REQ:		// 音声再生開始要求イベント
		// 音声再生開始処理
		bResult = Task_AudioPlayback_Start(g_tAudioPlayback.tReqInfo.eTaskKind, g_tAudioPlayback.tReqInfo.ePlaybackData);
		if (bResult == true)
		{
			// API側に「成功」を返す
			SendEvent(g_tAudioPlayback.tReqInfo.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_SUCCESS, portMAX_DELAY);

			// 「再生状態」へ遷移
			SetStageMap(&g_tAudioPlayback.StageMapHandle, TASK_AUDIOPLAYBAKC_STAGEMAP_PLAY);
		}
		else
		{
			// API側に「エラーイベント」を返す
			SendEvent(g_tAudioPlayback.tReqInfo.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_ERROR, portMAX_DELAY);
		}
		break;

	case AUDIO_PLAYBACK_EVENT_STOP_REQ:			// 音声再生停止要求イベント
		// 既に停止しているので、API側に「成功イベント」を返す
		SendEvent(g_tAudioPlayback.tReqInfo.eTaskKind, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_SUCCESS, portMAX_DELAY);
		break;

	default:
		break;
	}
}

//=================================================================================================
// 音声再生停止状態（終了処理)
//=================================================================================================
void Task_AudioPlayback_Stop_End(void)
{
}

