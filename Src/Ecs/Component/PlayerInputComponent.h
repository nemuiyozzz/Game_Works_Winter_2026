#pragma once

/// @brief プレイヤーの操作を受け付けるエンティティであることを示すコンポーネント
struct PlayerInputComponent
{
	// プレイヤーの行動ステータス関連
	float moveSpeed_ = 50.0f;	// 移動速度
	float jumpPower_ = 100.0f;	// ジャンプ力
};
