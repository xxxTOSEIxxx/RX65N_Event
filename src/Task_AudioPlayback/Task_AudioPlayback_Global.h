#ifndef TASK_AUDIOPLAYBACK_GLOBAL_H_
#define TASK_AUDIOPLAYBACK_GLOBAL_H_
#include "platform.h"
#include "Event.h"
#include "StageMap.h"
#include "r_adpcm.h"
#include "r_longq_if.h"
#include "AudioPlaybackApi.h"


#define AUDIOPLAYBACK_DECODE_SIZE	( 1000 )					// デコード変換用サイズ


//-------------------------------------------------
// 音声再生周波数種別
//-------------------------------------------------
typedef enum
{
	AUDIOPLAYBACK_FREQ_08000 = 0,								//  8.000KHz
	AUDIOPLAYBACK_FREQ_11025,									// 11.025KHz
	AUDIOPLAYBACK_FREQ_16000,									// 16.000KHz
	AUDIOPLAYBACK_FREQ_22050,									// 22.050KHz
	AUDIOPLAYBACK_FREQ_32000,									// 32.000KHz
	AUDIOPLAYBACK_FREQ_44100,									// 44.100KHz
	AUDIOPLAYBACK_FREQ_MAX										// == 終端 ==

} AUDIOPLAYBACK_FREQ_ENUM;


//-------------------------------------------------
// 音声再生状態種別
//-------------------------------------------------
typedef enum
{
	AUDIOPLAYBACK_STATUS_STOP = 0,								// 音声再生停止中
	AUDIOPLAYBACK_STATUS_PAUSE,									// 音声再生一時停止中
	AUDIOPLAYBACK_STATUS_PLAY,									// 音声再生再生中

} AUDIOPLAYBACK_STATUS_ENUM;



typedef struct
{
	QueueHandle_t						MutexHandle;			// 音声再生タスク用ミューテックスハンドル
	AUDIOPLAYBACK_REQ_INFO_TABLE		tReqInfo;				// 音声再生リクエスト情報

	adpcm_env							tAdpcmEnv;				// ADPCMデコード作業領域

	// 音声再生時に使用する
	AUDIOPLAYBACK_STATUS_ENUM			eStatus;				// 音声再生状態種別

	TASK_KIND_ENUM						eTaskKind;				// 音声再生要求元タスク（音声終了イベントを返す時に使用)
	AUDIOPLAYBACK_AUDIO_DATA_ENUM		eAudioData;				// 音声データ種別
	AUDIOPLAYBACK_FREQ_ENUM				eFreq;					// 音声データサンプリング周波数
	uint8_t* 							pAudioData;				// 音声データ格納位置
	uint32_t							AudioDataSize;			// 音声データサイズ
	uint32_t							DecodeIndex;			// ADPCMデータからPCMデータに変換した位置
	uint32_t							OutputIndex;			// 音声再生した位置

	longq_hdl_t							longqHandle;			// LONGキューのハンドル
	uint32_t							DecodeBuff[AUDIOPLAYBACK_DECODE_SIZE];

	STAGE_MAP_HANDLE					StageMapHandle;			// ステージマップハンドル

} TASK_AUDIOPLAYBACK_GLOBAL_TABLE;


#endif /* TASK_AUDIOPLAYBACK_GLOBAL_H_ */
