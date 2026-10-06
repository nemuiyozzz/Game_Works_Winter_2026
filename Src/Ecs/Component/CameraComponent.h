#pragma once

#include <DxLib.h>
#include "../../Ecs/Entity.h"

/// @brief カメラの追従設定や視野角を保持するコンポーネント
struct CameraComponent
{
	Entity targetEntity_ = NULL_ENTITY;
	VECTOR targetOffset_ = VGet(0.0f, 50.0f, 0.0f); // 注視点のズレ

	// 軌道カメラ用の設定関連
	float distance_ = 300.0f;                       // ターゲットからの距離
	float angleH_ = 0.0f;                           // 水平角度
	float angleV_ = 15.0f * (DX_PI_F / 180.0f);     // 垂直角度
	float rotationSpeed_ = 0.05f;                   // カメラの回転速度

	float fov_ = 60.0f * (DX_PI_F / 180.0f);
	float nearZ_ = 10.0f;
	float farZ_ = 20000.0f;
};