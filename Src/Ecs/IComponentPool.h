#pragma once

#include "Entity.h"

/// @brief 異なる型のプールを配列で管理するためのインターフェース
class IComponentPool
{
public:

	/// @brief 仮想デストラクタ
	virtual ~IComponentPool(void) = default;

	/// @brief エンティティが破棄された際、関連するデータを削除する
	/// @param entity 対象のエンティティID
	virtual void RemoveEntity(Entity entity) = 0;
};