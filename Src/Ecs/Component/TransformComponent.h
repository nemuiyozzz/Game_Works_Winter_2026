#pragma once

#include <DxLib.h>
#include "../../Common/Quaternion.h" 
#include "../../Utility/UtilityMath.h"

/// @brief 位置、回転、スケールのデータのみを保持するコンポーネント
struct TransformComponent
{
	// 基本情報関連
	VECTOR scale_ = UtilityMath::VECTOR_ONE;             // 大きさ
	VECTOR rotationEuler_ = UtilityMath::VECTOR_ZERO;    // 回転
	VECTOR position_ = UtilityMath::VECTOR_ZERO;         // ワールド位置
	VECTOR localPosition_ = UtilityMath::VECTOR_ZERO;    // ローカル位置
	VECTOR previousPosition_ = UtilityMath::VECTOR_ZERO; // 1フレーム前の位置

	// クォータニオン関連
	Quaternion rotation_ = Quaternion().Identity();      // ワールド回転
	Quaternion localRotation_ = Quaternion().Identity(); // ローカル回転

	// 行列関連
	MATRIX scaleMatrix_ = MGetIdent();    // スケール行列
	MATRIX rotationMatrix_ = MGetIdent(); // 回転行列
	MATRIX positionMatrix_ = MGetIdent(); // 位置行列
	MATRIX combinedMatrix_ = MGetIdent(); // 合成済み最終行列
};