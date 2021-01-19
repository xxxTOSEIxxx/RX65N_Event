//==============================================================================
// StageMap.h
//
// <変更履歴>
// 2020.10.15 [#---] M.Higuchi 		新規作成
//
//------------------------------------------------------------------------------
// Copyright(c) 2020 TOSEI Corporation All Rights Reserved.
// ※This source code or any portion thereof must not be reproduced or used in 
//   any manner whatsoever.
//==============================================================================
#ifndef STAGEMAP_H_
#define STAGEMAP_H_
#include <stdio.h>
#include "platform.h"

// ステージマップ処理関数定義
typedef void (*PSTAGE_MAP_FUNC) (void);

// ステージマップ構造体
typedef struct
{
    PSTAGE_MAP_FUNC             pInitFunc;      // 初期処理関数
    PSTAGE_MAP_FUNC             pProcFund;      // 処理関数
    PSTAGE_MAP_FUNC             pEndFund;       // 終了処理関数
} STAGE_MAP_TABLE;


// ステージマップハンドル構造体
typedef struct
{
    uint32_t                                       eStageMap;                       // ステージマップ種別
    uint32_t                                       ePreviousStageMap;               // 遷移前のステージマップ種別
    uint32_t                                       eNextStageMap;                   // 遷移予定のステージマップ種別（内部作業用）

    const STAGE_MAP_TABLE*                         ptStageMap;  					// ステージマップ

} STAGE_MAP_HANDLE;


//-----------------------------------------------------------------------------
// ステージマップ初期化
//-----------------------------------------------------------------------------
void InitStageMap(STAGE_MAP_HANDLE* phStageMap, const STAGE_MAP_TABLE* ptStageMap);


//-----------------------------------------------------------------------------
// ステージマップ遷移
//-----------------------------------------------------------------------------
void SetStageMap(STAGE_MAP_HANDLE* pStageMapHandle, uint32_t eStageMap);


//-----------------------------------------------------------------------------
// ステージマップ処理
//-----------------------------------------------------------------------------
void StageMapProc(STAGE_MAP_HANDLE* pStageMapHandle);


//-----------------------------------------------------------------------------
// ステージマップ取得
//-----------------------------------------------------------------------------
uint32_t GetStageMap(STAGE_MAP_HANDLE* pStageMapHandle);


#endif // #ifndef STAGEMAP_H_
