#pragma once

#include <DxLib.h>
#include "../../Common/Quaternion.h" 
#include "../../Utility/UtilityMath.h"

/// @brief 位置、回転、スケールのデータのみを保持するコンポーネント
struct TransformComponent
{
	// 基本情報関連
	VECTOR scale_;            // 大きさ
	VECTOR rotationEuler_;    // 回転（オイラー角表示用）
	VECTOR position_;         // ワールド位置
	VECTOR localPosition_;    // ローカル位置
	VECTOR previousPosition_; // 1フレーム前の位置

	// クォータニオン関連
	Quaternion rotation_;      // ワールド回転
	Quaternion localRotation_; // ローカル回転

	// 行列関連
	MATRIX scaleMatrix_;    // スケール行列
	MATRIX rotationMatrix_; // 回転行列
	MATRIX positionMatrix_; // 位置行列
	MATRIX combinedMatrix_; // 合成済み最終行列

	/// @brief コンストラクタ
	TransformComponent(void)
		: scale_(UtilityMath::VECTOR_ONE)
		, rotationEuler_(UtilityMath::VECTOR_ZERO)
		, position_(UtilityMath::VECTOR_ZERO)
		, localPosition_(UtilityMath::VECTOR_ZERO)
		, previousPosition_(UtilityMath::VECTOR_ZERO)
		, scaleMatrix_(MGetIdent())
		, rotationMatrix_(MGetIdent())
		, positionMatrix_(MGetIdent())
		, combinedMatrix_(MGetIdent())
		, rotation_(Quaternion().Identity())
		, localRotation_(Quaternion().Identity())
	{
	}
};