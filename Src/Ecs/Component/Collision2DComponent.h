#pragma once

#include <vector>
#include <memory>

#include "../Entity.h"
#include "../../Engine/Physics/Collider2D/Collider2DBase.h"

/// @brief 2Dコライダーデータと衝突履歴を保持するコンポーネント
struct Collision2DComponent
{
	// 2Dコライダーのリスト
	std::vector<std::shared_ptr<Collider2DBase>> colliders;

	// 衝突した相手のエンティティIDのリスト
	std::vector<Entity> hitEntities;
};