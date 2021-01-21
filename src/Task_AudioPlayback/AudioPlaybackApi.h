#ifndef AUDIOPLAYBACKAPI_H_
#define AUDIOPLAYBACKAPI_H_
#include "Task_def.h"
#include "stdbool.h"


//-------------------------------------------------
// 音声再生要求種別
//-------------------------------------------------
typedef enum
{
	AUDIOPLAYBACK_REQ_STOP = 0,							// 音声再生停止要求
	AUDIOPLAYBACK_REQ_PAUSE,							// 音声再生一時停止
	AUDIOPLAYBACK_REQ_START,							// 音声再生開始
	AUDIOPLAYBACK_REQ_RESTART,							// 音声再生再開

} AUDIOPLAYBACK_REQ_ENUM;

//-------------------------------------------------
// 音声再生データ種別
//-------------------------------------------------
typedef enum
{
	AUDIOPLAYBACK_AUDIO_DATA_001 = 0,					// 音声再生データ001
	AUDIOPLAYBACK_AUDIO_DATA_002,						// 音声再生データ002
	AUDIOPLAYBACK_AUDIO_DATA_003,						// 音声再生データ003
//	AUDIOPLAYBACK_AUDIO_DATA_004,						// 音声再生データ004
//	AUDIOPLAYBACK_AUDIO_DATA_005,						// 音声再生データ005
	AUDIOPLAYBACK_AUDIO_DATA_MAX						// === 終端 ===

} AUDIOPLAYBACK_AUDIO_DATA_ENUM;


//-------------------------------------------------
// 音声再生リクエスト情報構造体
//-------------------------------------------------
typedef struct
{
	TASK_KIND_ENUM					eTaskKind;			// 要求元タスク（音声終了イベントを返す時に使用)
	AUDIOPLAYBACK_AUDIO_DATA_ENUM	ePlaybackData;		// 音声再生データ種別

} AUDIOPLAYBACK_REQ_INFO_TABLE;


//=================================================================================================
// 音声再生開始要求
//=================================================================================================
bool AudioPlayback_Start(AUDIOPLAYBACK_REQ_INFO_TABLE* ptReqInfo);



#endif /* AUDIOPLAYBACKAPI_H_ */
