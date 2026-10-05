#pragma once

#include "ISystem.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief 3Dモデルの描画を管理・実行するシステムクラス
class ModelSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	ModelSystem(void) = default;

	/// @brief デストラクタ
	~ModelSystem(void) = default;

	/// @brief モデルを持つすべてのエンティティを探し出して一括描画する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief エンティティが持つ3Dモデルを描画する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void DrawModel(EcsRegistry& registry, Entity entity);
};