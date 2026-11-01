#pragma once

#include <DxLib.h>
#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief EnemyActionFacadeを介して全エネミーの扇形索敵・球状追跡を制御するシステムクラス
class EnemyAiSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	EnemyAiSystem(void) = default;

	/// @brief デストラクタ
	~EnemyAiSystem(void) = default;

	/// @brief 全エネミーのAI思考と移動を更新する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief デバッグ用に全エネミーの索敵範囲を描画する
	/// @param registry ECSデータベース
	static void DrawDebugVision(EcsRegistry& registry);

private:

	/// @brief 単一エネミーのAIステートを更新する
	/// @param registry ECSデータベース
	/// @param enemyEntity 対象のエネミーエンティティID
	/// @param playerPosition プレイヤーの現在座標
	void UpdateSingleEnemyAi(
		EcsRegistry& registry, Entity enemyEntity, const VECTOR& playerPosition);
};