#pragma once

#include <queue>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <vector>

#include "Entity.h"
#include "ComponentPool.h"

/// @brief エンティティとコンポーネントを一元管理するデータベースクラス
class EcsRegistry
{
public:

	/// @brief コンストラクタ
	EcsRegistry(void);

	/// @brief デストラクタ
	~EcsRegistry(void) = default;

	/// @brief 新しいエンティティを発行する
	/// @return 発行されたエンティティID
	Entity CreateEntity(void);

	/// @brief エンティティを破棄し、関連する全コンポーネントデータを削除する
	/// @param entity 破棄するエンティティID
	void DestroyEntity(Entity entity);

	/// @brief 指定したコンポーネントをエンティティに追加する
	/// @tparam TargetComponent 追加するコンポーネントの型
	/// @param entity 対象のエンティティID
	/// @param component 追加するデータ
	template <typename TargetComponent>
	void AddComponent(Entity entity, const TargetComponent& component)
	{
		GetComponentPool<TargetComponent>()->AddComponent(entity, component);
	}

	/// @brief 指定したコンポーネントのデータを取得する
	/// @tparam TargetComponent 取得するコンポーネントの型
	/// @param entity 対象のエンティティID
	/// @return データへの参照
	template <typename TargetComponent>
	TargetComponent& GetComponent(Entity entity)
	{
		return GetComponentPool<TargetComponent>()->GetComponent(entity);
	}

	/// @brief エンティティが指定したコンポーネントを持っているか確認する
	/// @tparam TargetComponent 確認するコンポーネントの型
	/// @param entity 対象のエンティティID
	/// @return 持っていればtrue
	template <typename TargetComponent>
	bool HasComponent(Entity entity)
	{
		return GetComponentPool<TargetComponent>()->HasComponent(entity);
	}

	/// @brief 指定した複数のコンポーネントをすべて持つエンティティのリストを取得する
	/// @tparam ...TargetComponents 条件となるコンポーネント群
	/// @return 条件を満たすエンティティIDのリスト
	template <typename... TargetComponents>
	std::vector<Entity> GetEntitiesWith(void)
	{
		std::vector<Entity> result;

		// 存在するすべてのエンティティをチェックする
		for (Entity entity = 0; entity < nextEntityId_; ++entity)
		{
			bool hasAll = (... && HasComponent<TargetComponents>(entity));

			if (hasAll)
			{
				result.push_back(entity);
			}
		}

		return result;
	}

private:

	// エンティティ管理関連
	Entity nextEntityId_;             // 次に発行する新規ID
	std::queue<Entity> freeEntities_; // 破棄されて再利用可能なIDのキュー

	// コンポーネント管理関連（型のインデックスをキーにプールを保持）
	std::unordered_map<std::type_index, std::shared_ptr<IComponentPool>> componentPools_;

	/// @brief 指定した型のコンポーネントプールを取得（なければ生成）する
	/// @tparam TargetComponent プールの対象となるコンポーネント型
	/// @return コンポーネントプールのポインタ
	template <typename TargetComponent>
	std::shared_ptr<ComponentPool<TargetComponent>> GetComponentPool(void)
	{
		std::type_index typeIndex(typeid(TargetComponent));

		// まだその型のプールが存在しなければ新規作成する
		if (componentPools_.find(typeIndex) == componentPools_.end())
		{
			componentPools_[typeIndex] = std::make_shared<ComponentPool<TargetComponent>>();
		}

		// 基底クラスのポインタを派生クラスにダウンキャストして返す
		return std::static_pointer_cast<ComponentPool<TargetComponent>>(componentPools_[typeIndex]);
	}
};