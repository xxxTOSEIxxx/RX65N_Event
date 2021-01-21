/*
 * Task_AudioPlayback_Stagemap_Stop.h
 *
 *  Created on: 2021/01/19
 *      Author: 99990045
 */

#ifndef TASK_AUDIOPLAYBACK_STAGEMAP_STOP_H_
#define TASK_AUDIOPLAYBACK_STAGEMAP_STOP_H_


//=================================================================================================
// 音声再生停止状態（初期化処理)
//=================================================================================================
void Task_AudioPlayback_Stop_Init(void);

//=================================================================================================
// 音声再生停止状態（メイン処理)
//=================================================================================================
void Task_AudioPlayback_Stop_Proc(void);

//=================================================================================================
// 音声再生停止状態（終了処理)
//=================================================================================================
void Task_AudioPlayback_Stop_End(void);


#endif /* TASK_AUDIOPLAYBACK_STAGEMAP_STOP_H_ */
