#pragma once

#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief 3Dモデルの描画を管理・実行するシステムクラス
class ModelSystem
{
public:

	/// @brief コンストラクタ
	ModelSystem(void) = default;

	/// @brief デストラクタ
	~ModelSystem(void) = default;

	/// @brief エンティティが持つ3Dモデルを描画する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void DrawModel(EcsRegistry& registry, Entity entity);
};