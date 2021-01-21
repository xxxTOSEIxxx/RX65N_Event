#include "Task_AudioPlayback_Global.h"
#include "Task_AudioPlayback_Stagemap.h"
#include "Event.h"
#include "r_gpio_rx_if.h"
#include "r_mpc_rx_if.h"
#include "Task_AudioPlayback_Main.h"
#include "string.h"


#define ENCODE_TEMP_STORAGE_SIZE_MIN				( 300 )
#define ENCODE_TEMP_STORAGE_SIZE					( 950 )
#define AUDIOPLAYBACK_PIN_DA						GPIO_PORT_0_PIN_5
//#define AUDIOPLAYBACK_PIN_SPEAKER					GPIO_PORT_J_PIN_2



TASK_AUDIOPLAYBACK_GLOBAL_TABLE					g_tAudioPlayback;



extern const uint8_t g_AudioData001_22050[37932];
extern const uint8_t g_AudioData002_22050[10086];
extern const uint8_t g_AudioData003_22050[28280];
//extern const uint8_t g_AudioData004_22050[27270];
//extern const uint8_t g_AudioData005_22050[21668];



typedef struct
{
	AUDIOPLAYBACK_FREQ_ENUM						eFreq;				// 音声データのサンプリングレート周波数
	uint32_t									DataSize;			// 音声データのデータサイズ
	const uint8_t*								pAudioData;			// 音声データ格納アドレス

} AUDIOPLAYBAKC_AUDIO_DATA_INFO_TABLE;


const AUDIOPLAYBAKC_AUDIO_DATA_INFO_TABLE		g_tAudioDataInfo[AUDIOPLAYBACK_AUDIO_DATA_MAX] =
{
	{ AUDIOPLAYBACK_FREQ_22050,			sizeof(g_AudioData001_22050),		g_AudioData001_22050		},
	{ AUDIOPLAYBACK_FREQ_22050,			sizeof(g_AudioData002_22050),		g_AudioData002_22050		},
	{ AUDIOPLAYBACK_FREQ_22050,			sizeof(g_AudioData003_22050),		g_AudioData003_22050		},
//	{ AUDIOPLAYBACK_FREQ_22050,			sizeof(g_AudioData004_22050),		g_AudioData004_22050		},
//	{ AUDIOPLAYBACK_FREQ_22050,			sizeof(g_AudioData005_22050),		g_AudioData005_22050		},
};




// 音声データのサンプリングレート周波数をMTUカウント値に変換(120MHzで算出)
const uint16_t		g_MtuCount[AUDIOPLAYBACK_FREQ_MAX] =
{
		15000,				// AUDIOPLAYBACK_FREQ_08000
		10884,				// AUDIOPLAYBACK_FREQ_11025
		 7500,				// AUDIOPLAYBACK_FREQ_16000
		 5442,				// AUDIOPLAYBACK_FREQ_22050
		 3750,				// AUDIOPLAYBACK_FREQ_32000
		 2721,				// AUDIOPLAYBACK_FREQ_44100
};







//=================================================================================================
// 音声再生タスク メイン処理
//=================================================================================================
void Task_AudioPlayback_Main(void)
{
	longq_err_t				LongqResult = LONGQ_SUCCESS;


	// 音声再生タスク用ミューテックスハンドルを生成
	g_tAudioPlayback.MutexHandle = xSemaphoreCreateMutex();

	// 音声再生タスク用イベント生成
	CreateEvent(TASK_KIND_AUDIO_PLAYBACK);

	// LONGキュー初期化
	LongqResult = R_LONGQ_Open(g_tAudioPlayback.DecodeBuff, AUDIOPLAYBACK_DECODE_SIZE, false, &g_tAudioPlayback.longqHandle);
	if (LongqResult != LONGQ_SUCCESS)
	{
		printf("R_LONGQ_Open Error. [LongqResult:%d]\n",LongqResult);
		while(1);
	}

	// デコードD/Aコンバータ初期化
	Task_AudioPlayback_Decode_DA_Converter_Init();

	// 音声データ出力タイミング用インターバルタイマー設定
	Task_AudioPlayback_Output_IntervalTimer_Init();

	// 音声再生タスク用ステージマップ初期化（最初の遷移先を「停止状態」とする）
	InitStageMap(&g_tAudioPlayback.StageMapHandle, g_tAudioPlaybackStagemap);
	SetStageMap(&g_tAudioPlayback.StageMapHandle, TASK_AUDIOPLAYBAKC_STAGEMAP_STOP);

	// ループ処理
	while( 1 )
	{
		// ステージマップ処理
		StageMapProc(&g_tAudioPlayback.StageMapHandle);
	}
}

//=================================================================================================
// 音声再生開始処理
//=================================================================================================
bool Task_AudioPlayback_Start(TASK_KIND_ENUM eKindTask, AUDIOPLAYBACK_AUDIO_DATA_ENUM eAudioData)
{
	int8_t					Result = 0;


	g_tAudioPlayback.eStatus = AUDIOPLAYBACK_STATUS_PLAY;
	g_tAudioPlayback.eTaskKind = eKindTask;
	g_tAudioPlayback.eAudioData = eAudioData;
	g_tAudioPlayback.eFreq = g_tAudioDataInfo[eAudioData].eFreq;
	g_tAudioPlayback.pAudioData = (uint8_t*)g_tAudioDataInfo[eAudioData].pAudioData;
	g_tAudioPlayback.AudioDataSize = g_tAudioDataInfo[eAudioData].DataSize;
	g_tAudioPlayback.DecodeIndex = 0;
	g_tAudioPlayback.OutputIndex = 0;

	// ADPCMデータ出コード初期化処理
	R_adpcm_initDec(&g_tAudioPlayback.tAdpcmEnv);

	// キューに格納されている情報をクリア
	R_LONGQ_Flush(g_tAudioPlayback.longqHandle);

	// エンコード処理
	Result = Task_AudioPlayback_Encode();
	if (Result == -1)
	{
		g_tAudioPlayback.eStatus = AUDIOPLAYBACK_STATUS_STOP;
		return false;
	}

	// デコードD/Aコンバータ開始
	Task_AudioPlayback_Decode_DA_Converter_Start();

	// 音声データ出力タイミング用インターバルタイマー開始
	Task_AudioPlayback_Output_IntervalTimer_Start();

	return true;
}


//=================================================================================================
// 音声再生停止処理
//=================================================================================================
void Task_AudioPlayback_Stop(void)
{
	// 音声データ出力タイミング用インターバルタイマー停止
	Task_AudioPlayback_Output_IntervalTimer_Stop();

	// デコードD/Aコンバータ停止
	Task_AudioPlayback_Decode_DA_Converter_Stop();

	// ADPCMデータ出コード初期化処理
	R_adpcm_initDec(&g_tAudioPlayback.tAdpcmEnv);

	// キューに格納されている情報をクリア
	R_LONGQ_Flush(g_tAudioPlayback.longqHandle);

	g_tAudioPlayback.eStatus = AUDIOPLAYBACK_STATUS_STOP;
//	g_tAudioPlayback.eTaskKind = eKindTask;
	g_tAudioPlayback.eAudioData = AUDIOPLAYBACK_AUDIO_DATA_MAX;
	g_tAudioPlayback.pAudioData = NULL;
	g_tAudioPlayback.AudioDataSize = 0;
	g_tAudioPlayback.DecodeIndex = 0;
	g_tAudioPlayback.OutputIndex = 0;
}

//=================================================================================================
//　エンコード処理
// ※戻り値： 0:音声変換データが一定サイズ保持された , 1：音声変換データが最後まで変換された , -1：異常
//=================================================================================================
int8_t Task_AudioPlayback_Encode(void)
{
	int8_t					Result = -1;
	uint16_t				StorageSize = 0;
	int16_t					DecodeData[4] = { 0x0000 };
	longq_err_t				LongqResult = LONGQ_SUCCESS;


	// 音声変換データが一定サイス保持される or 音声変換データが最後まで変換されるまでループを継続する
	while(1)
	{
		// LONGキューに登録されているデータ数を取得
		LongqResult = R_LONGQ_Used(g_tAudioPlayback.longqHandle, &StorageSize);
		if (LongqResult != LONGQ_SUCCESS)
		{
			Result = -1;
			break;
		}

		// エンコードデータが最後まで達した場合
		if (g_tAudioPlayback.DecodeIndex >= g_tAudioPlayback.AudioDataSize)
		{
			Result = 1;
			break;
		}

		// 一定サイズ保持した場合
		if (StorageSize >= ENCODE_TEMP_STORAGE_SIZE)
		{
			Result = 0;
			break;
		}

		// エンコード
		R_adpcm_refreshDec(&g_tAudioPlayback.pAudioData[g_tAudioPlayback.DecodeIndex], DecodeData, &g_tAudioPlayback.tAdpcmEnv);
		R_adpcm_decode(4,&g_tAudioPlayback.tAdpcmEnv);
		for(uint8_t i = 0 ; i < 4 ; i++)
		{
			DecodeData[i] = (uint16_t)(((DecodeData[i] + 0x0008) >> 4) + 0x0800);						// 16Bit PCMデータ → 12Bit D/Aデータ
			R_LONGQ_Put(g_tAudioPlayback.longqHandle,(uint32_t)DecodeData[i]);
		}
		g_tAudioPlayback.DecodeIndex += 2;
	}

	return Result;
}

//=================================================================================================
// デコードD/Aコンバータ初期化
//=================================================================================================
void Task_AudioPlayback_Decode_DA_Converter_Init(void)
{
	mpc_config_t				tMpcConfig;


	// 出力PINとして設定
	R_GPIO_PinDirectionSet(AUDIOPLAYBACK_PIN_DA, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinControl(AUDIOPLAYBACK_PIN_DA, GPIO_CMD_ASSIGN_TO_GPIO);

	// アナログ端子として設定
	memset(&tMpcConfig,0x00,sizeof(tMpcConfig));
	tMpcConfig.irq_enable = false;
	tMpcConfig.analog_enable = true;
	tMpcConfig.pin_function = 0;
	R_MPC_Write(AUDIOPLAYBACK_PIN_DA,&tMpcConfig);

	// 12Bit D/Aコンバータモジュールストップ状態解除
	SYSTEM.PRCR.WORD = 0xA502;
	MSTP_DA = 0;
	SYSTEM.PRCR.WORD = 0xA500;

	// D/A制御レジスタ(b7 - b0) : [チャンネル0,1のD/A変換を個別制御・チャンネル0のアナログ出力(DA0)を禁止・チャンネル1のアナログ出力(DA1)を禁止]
	DA.DACR.BYTE = 0x1F;					// b4 - b0 : 予約ビット						読むと"1"が読めます。書く場合は、"1"としてください
											// b5      : D/A許可ビット(DAE)				0:チャンネル0,1のD/A変換を個別制御
											//                          			1:チャンネル0,1のD/A変換を一括許可
											// b6	   : D/A出力許可0ビット(DAOE0)		0:チャンネル0のアナログ出力(DA0)を禁止
											//										1:チャンネル0のD/A変換を許可・チャンネル0のアナログ出力(DA0)を許可
											// b7	   : D/A出力許可0ビット(DAOE1)		0:チャンネル1のアナログ出力(DA1)を禁止
											//										1:チャンネル1のD/A変換を許可・チャンネル1のアナログ出力(DA1)を許可

	// データレジスタフォーマット選択レジスタ(b7 - b0) : [0:D/Aデータレジスタ右詰め]
	DA.DADPR.BYTE = 0x00;					// b6 - b0 : 予約ビット						読むと"0"が読めます。書く場合は、"0"としてください
											// b7      : フォーマット選択ビット(DPSEL)		0:D/Aデータレジスタ右詰め
											//         								1:D/Aデータレジスタ左詰め

	// D/Aデータレジスタ1
	DA.DADR1 = 0x0800;
}


//=================================================================================================
// デコードD/Aコンバータ開始
//=================================================================================================
void Task_AudioPlayback_Decode_DA_Converter_Start(void)
{
	// D/A出力許可1ビット(DAOE1) : [チャンネル1のD/A変換を許可・チャンネル0のアナログ出力(DA1)を許可]
	DA.DACR.BIT.DAOE1 = 1;
}


//=================================================================================================
// デコードD/Aコンバータ停止
//=================================================================================================
void Task_AudioPlayback_Decode_DA_Converter_Stop(void)
{
	// D/A出力許可1ビット(DAOE1) : [チャンネル1のアナログ出力(DA0)を禁止]
	DA.DACR.BIT.DAOE1 = 0;
}


//=================================================================================================
// デコードD/Aコンバータスリープ
//=================================================================================================
void Task_AudioPlayback_Decode_DA_Converter_Sleep(void)
{
	// 12Bit D/Aコンバータモジュールストップ状態へ移行
	SYSTEM.PRCR.WORD = 0xA502;
	MSTP_DA = 1;
	SYSTEM.PRCR.WORD = 0xA500;
}


//=================================================================================================
// 音声データ出力タイミング用インターバルタイマー設定
//=================================================================================================
void Task_AudioPlayback_Output_IntervalTimer_Init(void)
{
	// マルチファンクションタイマーパルスユニットモジュールストップ状態解除
	SYSTEM.PRCR.WORD = 0xA502;
	MSTP_MTU = 0;
	SYSTEM.PRCR.WORD = 0xA500;

	// MTU1  タイマコントロールレジスタ(b7 - b0)
	MTU1.TCR.BYTE = 0x20;					// b7 - b5 : カウンタクリア要因選択ビット(CCLR)
											// b4 - b3 : クロックエッジ選択ビット(CKEG)			0,0 : 立ち上がりエッジでカウント
											//											0,1 : 立ち下がりエッジでカウント
											//											1,x : 両エッジでカウント

	// MTU1 タイマモードレジスタ(b7 - b0)
	MTU1.TMDR1.BYTE	= 0x00;					// b7      : 予約ビット							読むと"0"が読めます。書く場合は、"0"としてください
											// b6	   : バッファ動作Eビット(BFE)				0:MTU0.TGREとMTU0.TGRFは通常動作
											// 											1:MTU0.TGREとMTU0.TGRFはバッファ動作
											// b5	   : バッファ動作Bビット(BFB)				0:MTU0.TGRBとMTU0.TGRDは通常動作
											// 											1:MTU0.TGRBとMTU0.TGRDはバッファ動作
											// b4	   : バッファ動作Aビット(BFA)				0:MTU0.TGRAとMTU0.TGRCは通常動作
											// 											1:MTU0.TGRAとMTU0.TGRCはバッファ動作
											// b3 - b0 : モード選択ビット

	// MTU1 タイマI/Oコントロールレジスタ
	MTU1.TIOR.BYTE  = 0x00;					// b7 - b4 : I/OコントロールBビット(IOB)
											// b3 - b0 : I/OコントロールAビット(IOA)

	// タイマインタラプトイネーブルレジスタ
	MTU1.TIER.BYTE	= 0x01;					// b7      : A/D変換開始要求許可ビット(TTGE)		0:A/D変換開始要求の発生を禁止
											//											1:A/D変換開始要求の発生を許可
											// b6      : A/D変換開始要求許可2ビット(TTGE2)	0:MTUn.TCNTのアンダーフロー（谷)によるA/D変換要求を禁止
											//											1:MTUn.TCNTのアンダーフロー（谷)によるA/D変換要求を許可
											// b5      : アンダーフロー割込み許可ビット(TCIEU)	0:割込み要求(TCIU)を禁止
											// 											1:割込み要求(TCIU)を許可
											// b4      : オーバーフロー割込み許可ビット(TCIEV)	0:割込み要求(TCIV)を禁止
											// 											1:割込み要求(TCIV)を許可
											// b3      : TGR割り込み許可Dビット(TGIED)		0:割込み要求(TCID)を禁止
											// 											1:割込み要求(TCID)を許可
											// b2      : TGR割り込み許可Cビット(TGIEC)		0:割込み要求(TCIC)を禁止
											// 											1:割込み要求(TCIC)を許可
											// b1      : TGR割り込み許可Bビット(TGIEB)		0:割込み要求(TCIB)を禁止
											// 											1:割込み要求(TCIB)を許可
											// b0      : TGR割り込み許可Aビット(TGIEA)		0:割込み要求(TCIA)を禁止
											// 											1:割込み要求(TCIA)を許可

	MTU1.TCNT	= 0x0000;
	MTU1.TGRA	= 0x0000;
	MTU1.TGRB  	= 0x0000;

	IEN(MTU1, TGIA1) = 0;					// 割り込み禁止
	IPR(MTU1, TGIA1) = 3;					// 割り込み優先度
	IR(MTU1, TGIA1) = 0;					// 割り込み要求
	IEN(MTU1, TGIA1) = 1;					// 割り込み許可
}


//=================================================================================================
// 音声データ出力タイミング用インターバルタイマー開始
//=================================================================================================
void Task_AudioPlayback_Output_IntervalTimer_Start(void)
{
	MTU1.TCNT	= 0x0000;
	MTU1.TGRA	= g_MtuCount[g_tAudioPlayback.eFreq];

	// MTU1のカウントを開始
	MTU.TSTRA.BIT.CST1 = 1;
}


//=================================================================================================
// 音声データ出力タイミング用インターバルタイマー停止
//=================================================================================================
void Task_AudioPlayback_Output_IntervalTimer_Stop(void)
{
	// MTU1のカウントを停止
	MTU.TSTRA.BIT.CST1 = 0;
}


//=================================================================================================
// 音声データ出力タイミング用インターバルタイマースリープ
//=================================================================================================
void Task_AudioPlayback_Output_IntervalTimer_Sleep(void)
{
	IEN(MTU1, TGIA1) = 0;					// 割り込み禁止
	IPR(MTU1, TGIA1) = 0;					// 割り込み優先度
	IR(MTU1, TGIA1) = 0;					// 割り込み要求

	// マルチファンクションタイマーパルスユニットモジュールストップ状態へ遷移
	SYSTEM.PRCR.WORD = 0xA502;
	MSTP_MTU = 1;
	SYSTEM.PRCR.WORD = 0xA500;
}


//=================================================================================================
// タイマー割り込み処理
//=================================================================================================
#pragma interrupt R_interrupt_decode_timer(vect=VECT(MTU1,TGIA1))
void R_interrupt_decode_timer(void)
{
	longq_err_t				LongqResult = LONGQ_SUCCESS;
	uint32_t				AudioData = 0;
	BaseType_t				xHigherPriorityTaskWoken = pdFALSE;

	// 音声出力インデックスが音声データサイズ以上となった場合
	if ((g_tAudioPlayback.AudioDataSize * 2) <= g_tAudioPlayback.OutputIndex)
	{
		if (g_tAudioPlayback.eTaskKind != TASK_KIND_AUDIO_PLAYBACK)
		{
			Task_AudioPlayback_Stop();
			g_tAudioPlayback.eTaskKind = TASK_KIND_AUDIO_PLAYBACK;
			SendEventFromISR(TASK_KIND_AUDIO_PLAYBACK, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_STOP_REQ, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		return;
	}

	// LONGキューから音声出力エンコードデータを取得
	LongqResult = R_LONGQ_Get(g_tAudioPlayback.longqHandle,&AudioData);
	if (LongqResult != LONGQ_SUCCESS)
	{
		if (g_tAudioPlayback.eTaskKind != TASK_KIND_AUDIO_PLAYBACK)
		{
			Task_AudioPlayback_Stop();
			g_tAudioPlayback.eTaskKind = TASK_KIND_AUDIO_PLAYBACK;
			SendEventFromISR(TASK_KIND_AUDIO_PLAYBACK, PRIORITY_KIND_NORMAL, AUDIO_PLAYBACK_EVENT_STOP_REQ, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		return;
	}

	// 音声出力
	DA.DADR1 = (uint16_t)AudioData;
	g_tAudioPlayback.OutputIndex++;
}


