#ifndef TASK_AUDIOPLAYBACK_STAGEMAP_H_
#define TASK_AUDIOPLAYBACK_STAGEMAP_H_
#include "Task_AudioPlayback_Stagemap_Stop.h"
#include "Task_AudioPlayback_Stagemap_Play.h"
#include "StageMap.h"


// AudioPlaybackタスクステージマップ種別
typedef enum
{
	TASK_AUDIOPLAYBAKC_STAGEMAP_STOP = 0,					// 音声再生停止状態
	TASK_AUDIOPLAYBAKC_STAGEMAP_PLAY,						// 音声再生再生中状態
	TASK_AUDIOPLAYBAKC_STAGEMAP_MAX							// === 終端 ===

} TASK_AUDIOPLAYBAKC_STAGEMAP_ENUM;



// AudioPlaybackタスクステージマップ定義
static const STAGE_MAP_TABLE g_tAudioPlaybackStagemap[TASK_AUDIOPLAYBAKC_STAGEMAP_MAX] =
{
	{ Task_AudioPlayback_Stop_Init,				Task_AudioPlayback_Stop_Proc,				Task_AudioPlayback_Stop_End					},		// 音声再生停止状態
	{ Task_AudioPlayback_Play_Init,				Task_AudioPlayback_Play_Proc,				Task_AudioPlayback_Play_End					},		// 音声再生再生中状態
};


#endif /* TASK_AUDIOPLAYBACK_STAGEMAP_H_ */
