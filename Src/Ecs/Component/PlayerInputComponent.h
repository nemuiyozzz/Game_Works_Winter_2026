#pragma once

/// @brief プレイヤーの操作を受け付けるエンティティであることを示すコンポーネント
struct PlayerInputComponent
{
	// プレイヤーの行動ステータス関連
	float moveSpeed = 50.0f;	// 移動速度
	float jumpPower = 100.0f;	// ジャンプ力
};
