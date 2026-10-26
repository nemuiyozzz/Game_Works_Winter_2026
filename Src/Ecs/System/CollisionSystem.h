#pragma once

#include <vector>
#include <memory>

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"
#include "../../Engine/Physics/Collision/CollisionInfo.h"
#include "../../Engine/Physics/Collider/ColliderBase.h"

class ColliderBase;

/// @brief 衝突判定と押し戻し処理を行うシステムクラス
class CollisionSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	CollisionSystem(void) = default;

	/// @brief デストラクタ 
	~CollisionSystem(void) override = default;

	/// @brief 更新処理
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief 指定エンティティが特定のタグを持つコライダーと衝突しているか判定する
	/// @param registry ECSデータベース
	/// @param targetEntity 判定元のエンティティID
	/// @param targetTag 衝突を確認したい相手のコライダーのタグ
	/// @return 衝突していれば true
	static bool CheckHitByTag(EcsRegistry& registry, Entity targetEntity,
		ColliderBase::TAG targetTag);

private:

	/// @brief コライダーの追従ターゲットを更新し、履歴をクリアする
	/// @param registry ECSデータベース
	/// @param entities 対象のエンティティリスト
	void UpdateCollidersTarget(EcsRegistry& registry, const std::vector<Entity>& entities);

	/// @brief すべてのエンティティ間の総当たり判定を行う
	/// @param registry ECSデータベース
	/// @param entities 対象のエンティティリスト
	void CheckAllCollisions(EcsRegistry& registry, const std::vector<Entity>& entities);

	/// @brief 2つのコライダー間の衝突を処理する
	/// @param registry ECSデータベース
	/// @param entityA エンティティA
	/// @param entityB エンティティB
	/// @param colliderA コライダーA
	/// @param colliderB コライダーB
	void ProcessCollisionPair(EcsRegistry& registry, Entity entityA, Entity entityB,
		std::shared_ptr<ColliderBase>& colliderA, std::shared_ptr<ColliderBase>& colliderB);

	/// @brief 衝突結果をもとにエンティティのめり込みを解消する
	/// @param registry ECSデータベース
	/// @param entityA 衝突したエンティティA
	/// @param entityB 衝突したエンティティB
	/// @param info 衝突情報
	void ResolvePenetration(EcsRegistry& registry, Entity entityA, Entity entityB,
		const CollisionInfo& info);

};

