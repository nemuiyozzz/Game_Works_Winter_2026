#pragma once

#include "StageBase.h"

/// @brief 訓練場ステージクラス
class StageTraining : public StageBase
{
public:

	// プレイヤーの初期スポーン座標
	static constexpr VECTOR SPAWN_POSITION = { 500.0f, 0.0f, 500.0f };

	/// @brief コンストラクタ
	/// @param registry ECSデータベースの参照
	StageTraining(EcsRegistry& registry);

	/// @brief デストラクタ
	~StageTraining(void) override = default;

	/// @brief 非同期ロード処理
	void Load(void) override;

	/// @brief 初期化処理
	void Initialize(void) override;

	/// @brief 更新処理
	void Update(void) override;

	/// @brief 描画処理
	void Draw(void) override;

	/// @brief プレイヤーの初期スポーン座標を取得する
	/// @return 初期スポーン座標
	VECTOR GetPlayerSpawnPosition(void) const override;
};