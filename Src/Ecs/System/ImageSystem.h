#pragma once

#include "ISystem.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief 2D画像の描画を管理・実行するシステムクラス
class ImageSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	ImageSystem(void) = default;

	/// @brief デストラクタ
	~ImageSystem(void) = default;

	/// @brief 画像を持つすべてのエンティティを探し出して一括描画する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief エンティティが持つ2D画像を描画する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void DrawImage(EcsRegistry& registry, Entity entity);
};