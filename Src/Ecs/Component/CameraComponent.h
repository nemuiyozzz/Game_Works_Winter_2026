#pragma once

#include <DxLib.h>

#include "../Entity.h"

class CameraComponent
{
public:

	// 追従対象関連
	Entity targetEntity_ = NULL_ENTITY;				// 追従対象のエンティティID
	VECTOR offset_ = VGet(0.0f, 150.0f, -400.0f);	// ターゲットからの相対座標
	VECTOR targetOffset_ = VGet(0.0f, 60.0f, 0.0f);	// 注視点のずれ

	// 視野関連
	float fov_ = 60.0f * (DX_PI_F / 180.0f);	// 視野角
	float nearZ_ = 10.0f;						// 描画最短距離
	float farZ_ = 20000.0f;						// 描画最大距離
};