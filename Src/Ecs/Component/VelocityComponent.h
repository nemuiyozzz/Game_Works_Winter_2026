#pragma once

#include <DxLib.h>
#include "../../Engine/Utility/UtilityMath.h"

/// @brief 移動に関する速度や加速度のデータを保持するコンポーネント
struct VelocityComponent
{
	// 速度・加速度関連
	VECTOR velocity = UtilityMath::VECTOR_ZERO;		// 現在の移動速度ベクトル
	VECTOR acceleration = UtilityMath::VECTOR_ZERO;	// 加速度ベク1トル
	float maxSpeed = -1.0f;							// 最大制限速度
};
