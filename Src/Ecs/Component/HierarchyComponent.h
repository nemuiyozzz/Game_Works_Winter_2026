#pragma once

#include <vector>

#include "../Entity.h"

/// @brief エンティティ間の親子関係を保持するコンポーネント
struct HierarchyComponent
{
	Entity parent = NULL_ENTITY;		// 親となるエンティティのID
	std::vector<Entity> children = {};	// 子となるエンティティのIDリスト
};