#pragma once

#include <vector>
#include <string>

#include "../Entity.h"

/// @brief エンティティ間の親子関係を保持するコンポーネント
struct HierarchyComponent
{
	Entity parent = NULL_ENTITY;		// 親となるエンティティのID
	std::vector<Entity> children = {};	// 子となるエンティティのIDリスト

	std::wstring attachBoneName = L"";	// 追従するボーン名
	int attachFrameIndex = -1;			// 検索済みのフレーム番号キャッシュ
};