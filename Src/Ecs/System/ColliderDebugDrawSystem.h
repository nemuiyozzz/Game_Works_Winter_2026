#pragma once

#include "../../Ecs/EcsRegistry.h"

/// @brief コライダーのデバッグ描画を一括管理するシステム
class ColliderDebugDrawSystem
{
public:

	/// @brief コンストラクタ
	ColliderDebugDrawSystem(void) = default;

	/// @brief デストラクタ
	~ColliderDebugDrawSystem(void) = default;

	/// @brief デバッグ表示の切り替え入力を更新する
	void Update(void);

	/// @brief コライダーのデバッグ描画を行う
	/// @param registry ECSデータベース
	void Draw(EcsRegistry& registry) const;

private:

	bool isDebugDrawActive_ = false; // デバッグ表示フラグ
	bool wasKeyPressed_ = false;     // 前フレームのキー状態（トリガー判定用）
};