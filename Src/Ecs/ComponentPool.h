#pragma once

#include <unordered_map>
#include "Entity.h"
#include "IComponentPool.h"

/// @brief 特定のコンポーネントのデータを管理するプールクラス
/// @tparam TargetComponent 管理するコンポーネントの型
template <typename TargetComponent>
class ComponentPool : public IComponentPool
{
public:

	/// @brief デフォルトコンストラクタ
	ComponentPool(void) = default;

	/// @brief デストラクタ
	~ComponentPool(void) override = default;

	/// @brief コンポーネントのデータを追加または上書きする
	/// @param entity 対象のエンティティID
	/// @param component 追加するデータ
	void AddComponent(Entity entity, const TargetComponent& component)
	{
		componentData_[entity] = component;
	}

	/// @brief コンポーネントのデータを取得する
	/// @param entity 対象のエンティティID
	/// @return コンポーネントのデータへの参照
	TargetComponent& GetComponent(Entity entity)
	{
		return componentData_[entity];
	}

	/// @brief エンティティのデータが存在するか確認する
	/// @param entity 対象のエンティティID
	/// @return 存在すればtrue
	bool HasComponent(Entity entity) const
	{
		return componentData_.find(entity) != componentData_.end();
	}

	/// @brief エンティティのデータを削除する
	/// @param entity 対象のエンティティID
	void RemoveEntity(Entity entity) override
	{
		componentData_.erase(entity);
	}

private:

	// エンティティIDとデータを紐づけるマップ
	std::unordered_map<Entity, TargetComponent> componentData_;

};