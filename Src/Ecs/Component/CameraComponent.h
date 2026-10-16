#pragma once

#include <DxLib.h>

#include "../../Ecs/Entity.h"

/// @brief カメラのモード
enum class CAMERA_MODE
{
	TRACKING, // 追従カメラ
	FREE,     // フリーカメラ
};

/// @brief カメラの追従設定や視野角を保持するコンポーネント
struct CameraComponent
{
	// モード関連
	CAMERA_MODE mode = CAMERA_MODE::TRACKING;      // 現在のカメラモード

	// 追従カメラ関連
	Entity targetEntity = NULL_ENTITY;
	VECTOR targetOffset = VGet(0.0f, 50.0f, 0.0f); // 注視点のズレ
	float distance = 300.0f;                       // ターゲットからの距離

	// 共通・回転関連
	float angleH = 0.0f;                           // 水平角度
	float angleV = 15.0f * (DX_PI_F / 180.0f);     // 垂直角度
	float rotationSpeed = 0.05f;                   // カメラの回転速度
	bool isCursorLocked = true;                    // マウスを画面中央に固定するか

	// フリーカメラ関連
	float freeMoveSpeed = 2.0f;                    // フリーカメラの通常移動速度

	// 描画設定関連
	float fov = 60.0f * (DX_PI_F / 180.0f);
	float nearZ = 1.0f;
	float farZ = 20000.0f;
};