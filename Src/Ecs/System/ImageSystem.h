#pragma once

#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief 2D画像の描画を管理・実行するシステムクラス
class ImageSystem
{
public:

	/// @brief コンストラクタ
	ImageSystem(void) = default;

	/// @brief デストラクタ
	~ImageSystem(void) = default;

	/// @brief エンティティが持つ2D画像を描画する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void DrawImage(EcsRegistry& registry, Entity entity);
};