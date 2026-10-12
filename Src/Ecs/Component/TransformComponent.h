#pragma once

#include <DxLib.h>
#include "../../Common/Quaternion.h" 
#include "../../Engine/Utility/UtilityMath.h"

/// @brief 位置、回転、スケールのデータのみを保持するコンポーネント
struct TransformComponent
{
	// 基本情報関連
	VECTOR scale = UtilityMath::VECTOR_ONE;             // 大きさ
	VECTOR rotationEuler = UtilityMath::VECTOR_ZERO;    // 回転
	VECTOR position = UtilityMath::VECTOR_ZERO;         // ワールド位置
	VECTOR localPosition = UtilityMath::VECTOR_ZERO;    // ローカル位置
	VECTOR previousPosition = UtilityMath::VECTOR_ZERO; // 1フレーム前の位置

	// クォータニオン関連
	Quaternion rotation_ = Quaternion().Identity();      // ワールド回転
	Quaternion localRotation = Quaternion().Identity(); // ローカル回転

	// 行列関連
	MATRIX scaleMatrix = MGetIdent();    // スケール行列
	MATRIX rotationMatrix = MGetIdent(); // 回転行列
	MATRIX positionMatrix = MGetIdent(); // 位置行列
	MATRIX combinedMatrix = MGetIdent(); // 合成済み最終行列
};