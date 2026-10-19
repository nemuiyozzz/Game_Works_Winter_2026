#pragma once

#include <vector>
#include <memory>
#include <string>

#include "EcsRegistry.h"
#include "System/ISystem.h"

class ColliderDebugDrawSystem;

/// @brief EcsRegistryと全Systemの実行順序を一元管理するファサードクラス
class EcsWorld
{
public:

	/// @brief コンストラクタ
	EcsWorld(void) = default;

	/// @brief デストラクタ
	~EcsWorld(void) = default;

	/// @brief 各種システムの生成と登録を行う
	void Initialize(void);

	/// @brief 更新用システムを一括実行する
	void Update(void);

	/// @brief 描画用システムを一括実行する
	void Draw(void);

	/// @brief 管理しているシステムを解放する
	void Release(void);

	/// @brief 更新用システムを外部から追加登録する
	/// @tparam TargetSystem 追加するシステムの型
	/// @tparam ...Args コンストラクタ引数の型
	/// @param ...args システムのコンストラクタに渡す引数
	template<typename TargetSystem, typename... Args>
	void AddUpdateSystem(Args&&... args)
	{
		updateSystems_.push_back(std::make_shared<TargetSystem>(std::forward<Args>(args)...));
	}

	/// @brief 描画用システムを外部から追加登録する
	/// @tparam TargetSystem 追加する描画システムの型
	/// @tparam ...Args コンストラクタ引数の型
	/// @param ...args システムのコンストラクタに渡す引数
	template <typename TargetSystem, typename... Args>
	void AddDrawSystem(Args&&... args)
	{
		drawSystems_.push_back(std::make_shared<TargetSystem>(std::forward<Args>(args)...));
	}

	/// @brief 登録されたレシピ名からエンティティを生成する
	/// @param actorName レシピ名
	/// @return 生成されたエンティティID
	Entity SpawnActor(const std::wstring& actorName);

	/// @brief 追従対象を指定してカメラエンティティを生成する
	/// @param targetEntity 追従対象のエンティティID
	/// @return 生成されたカメラのエンティティID
	Entity SpawnCamera(Entity targetEntity);

	/// @brief 指定したエンティティに個別のコンポーネントを追加する
	/// @tparam TargetComponent 追加するコンポーネントの型
	/// @param entity 対象のエンティティID
	/// @param component 追加するデータ
	template <typename TargetComponent>
	void AttachComponent(Entity entity, const TargetComponent& component)
	{
		ecsRegistry_.AddComponent<TargetComponent>(entity, component);
	}

	/// @brief 内部のECSレジストリを取得する
	/// @return EcsRegistryの参照
	EcsRegistry& GetRegistry(void) { return ecsRegistry_; }

private:

	// ECS・システム管理関連
	EcsRegistry ecsRegistry_;											// エンティティ・コンポーネント管理
	std::vector<std::shared_ptr<ISystem>> updateSystems_;				// 更新用システム一覧
	std::vector<std::shared_ptr<ISystem>> drawSystems_;					// 描画用システム一覧
	std::shared_ptr<ColliderDebugDrawSystem> colliderDebugDrawSystem_;	// コライダーデバッグ描画
};