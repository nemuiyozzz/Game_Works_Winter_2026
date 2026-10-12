#pragma once

#include <vector>
#include <memory>

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

class Collider2DBase;

class Collision2DSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	Collision2DSystem(void) = default;

	/// @brief デストラクタ 
	~Collision2DSystem(void) override = default;
	
	/// @brief 2D衝突判定の更新を行う
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

private:

	/// @brief 衝突履歴の初期化を行う
	/// @param registry ECSデータベース
	/// @param entities 対象のエンティティリスト
	void ClearHitHistories(EcsRegistry& registry, const std::vector<Entity>& entities);

	/// @brief すべてのエンティティ間の2D総当たり判定を行う
	/// @param registry ECSデータベース
	/// @param entities 対象のエンティティリスト
	void CheckAllCollisions(EcsRegistry& registry, const std::vector<Entity>& entities);

	/// @brief 2つの2Dコライダー間の衝突を処理する
	/// @param registry ECSデータベース
	/// @param entityA エンティティA
	/// @param entityB エンティティB
	/// @param colliderA 2DコライダーA
	/// @param colliderB 2DコライダーB
	void ProcessCollisionPair(EcsRegistry& registry, Entity entityA, Entity entityB,
		std::shared_ptr<Collider2DBase>& colliderA, std::shared_ptr<Collider2DBase>& colliderB);

};

