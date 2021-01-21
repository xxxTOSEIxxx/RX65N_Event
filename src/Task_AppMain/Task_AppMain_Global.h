#ifndef TASK_APPMAIN_GLOBAL_H_
#define TASK_APPMAIN_GLOBAL_H_
#include "platform.h"
#include "StageMap.h"
#include "Event.h"
#include "Timer.h"
#include "AudioPlaybackApi.h"

//-------------------------------------------------
// AppMain用グローバル変数構造体
//-------------------------------------------------
typedef struct
{
	STAGE_MAP_HANDLE					StageMapHandle;		// ステージマップハンドル

	TimerHandle_t						TimerHandle;		// タイマーハンドル


	AUDIOPLAYBACK_AUDIO_DATA_ENUM		eAudioData;

} TASK_APP_MAIN_GLOBAL_TABLE;


#endif /* TASK_APPMAIN_GLOBAL_H_ */
