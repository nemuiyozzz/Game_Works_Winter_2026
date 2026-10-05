#pragma once

#include <DxLib.h>
#include "../../Utility/UtilityMath.h"

/// @brief 移動に関する速度や加速度のデータを保持するコンポーネント
struct VelocityComponent
{
	// 速度・加速度関連
	VECTOR velocity_ = UtilityMath::VECTOR_ZERO;		// 現在の移動速度ベクトル
	VECTOR acceleration_ = UtilityMath::VECTOR_ZERO;	// 加速度ベク1トル
	float maxSpeed_ = -1.0f;							// 最大制限速度
};
