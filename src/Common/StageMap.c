//==============================================================================
// StageMap.cpp
//
// <変更履歴>
// 2020.10.15 [#---] M.Higuchi 		新規作成
//
//------------------------------------------------------------------------------
// Copyright(c) 2020 TOSEI Corporation All Rights Reserved.
// ※This source code or any portion thereof must not be reproduced or used in 
//   any manner whatsoever.
//==============================================================================
#include "StageMap.h"


#define STAGE_MAP_KIND_INIT              ( 0xFFFFFFFF )                             // ステージマップ種別の初期値


// ステージマップ関数種別
typedef enum
{
    STAGE_MAP_FUNC_INIT = 0,                // 初期化処理
    STAGE_MAP_FUNC_PROC,                    // メイン処理
    STAGE_MAP_FUNC_END,                     // 終了処理
    STAGE_MAP_FUNC_MAX,                     // === 終端 ===
} STAGE_MAP_FUNC_ENUM;


//-----------------------------------------------------------------------------
// ステージマップ初期化
//-----------------------------------------------------------------------------
void InitStageMap(STAGE_MAP_HANDLE* pStageMapHandle, const STAGE_MAP_TABLE* ptStageMap)
{
    // 引数チェック
    if ((ptStageMap == NULL) || (pStageMapHandle == NULL))
    {
        return;
    }

    // 各ステージマップ種別を初期化
    pStageMapHandle->eStageMap = STAGE_MAP_KIND_INIT;
    pStageMapHandle->ePreviousStageMap = STAGE_MAP_KIND_INIT;
    pStageMapHandle->eNextStageMap = STAGE_MAP_KIND_INIT;

    // ステージマップ設定
    pStageMapHandle->ptStageMap = (STAGE_MAP_TABLE*)ptStageMap;
}



//-----------------------------------------------------------------------------
// ステージマップ遷移
//-----------------------------------------------------------------------------
void SetStageMap(STAGE_MAP_HANDLE* pStageMapHandle, uint32_t eStageMap)
{
    // 引数チェック
    if (pStageMapHandle == NULL)
    {
        return;
    }

    // 次の遷移先が現状のマップと違う場合
    if (pStageMapHandle->eStageMap != eStageMap)
    {
        // 指定された次の遷移先を保持する
        pStageMapHandle->eNextStageMap = eStageMap;
    }
}


//-----------------------------------------------------------------------------
// ステージマップ処理
//-----------------------------------------------------------------------------
void StageMapProc(STAGE_MAP_HANDLE* pStageMapHandle)
{
    PSTAGE_MAP_FUNC         pStageMapInitFunc = NULL;
    PSTAGE_MAP_FUNC         pStageMapProcFunc = NULL;
    PSTAGE_MAP_FUNC         pStageMapEndFunc = NULL;


    // 遷移先が変更されている場合
    if (pStageMapHandle->eNextStageMap != pStageMapHandle->eStageMap)
    {
        // ステージマップを変更する
        pStageMapHandle->ePreviousStageMap = pStageMapHandle->eStageMap;
        pStageMapHandle->eStageMap = pStageMapHandle->eNextStageMap;

        // 遷移先の初期処理を行う
        pStageMapInitFunc = pStageMapHandle->ptStageMap[pStageMapHandle->eStageMap].pInitFunc;
        if (pStageMapInitFunc != NULL)
        {
            pStageMapInitFunc();
        }
    }

    // 遷移先のメイン処理を行う
    pStageMapProcFunc = pStageMapHandle->ptStageMap[pStageMapHandle->eStageMap].pProcFund;
    if (pStageMapProcFunc != NULL)
    {
        pStageMapProcFunc();
    }

    // 次の遷移先が現状のマップと違う場合、現状のステージマップの終了処理を行う
    if (pStageMapHandle->eNextStageMap != pStageMapHandle->eStageMap)
    {
        pStageMapEndFunc = pStageMapHandle->ptStageMap[pStageMapHandle->eStageMap].pEndFund;
        if (pStageMapEndFunc != NULL)
        {
            pStageMapEndFunc();
        }
    }
}


//-----------------------------------------------------------------------------
// ステージマップ取得
//-----------------------------------------------------------------------------
uint32_t GetStageMap(STAGE_MAP_HANDLE* pStageMapHandle)
{
	// 引数チェック
	if (pStageMapHandle == NULL)
	{
		return 0;
	}

	return pStageMapHandle->eStageMap;
}

