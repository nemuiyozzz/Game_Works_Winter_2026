#pragma once

#include <vector>
#include <memory>
#include "../Entity.h"
#include "../../Engine/Physics/Collider/ColliderBase.h"

/// @brief コライダーデータと衝突履歴を保持するコンポーネント
struct CollisionComponent
{
	// コライダーリスト
	std::vector<std::shared_ptr<ColliderBase>> colliders_;

	// 衝突したエンティティリスト
	std::vector<Entity> hitEntities_;
};