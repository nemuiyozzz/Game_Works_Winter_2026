#pragma once

#include <DxLib.h>

#include "ISystem.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief VelocityComponentのデータをもとにTransformを更新するシステムクラス
class MovementSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	MovementSystem(void) = default;

	/// @brief デストラクタ 
	~MovementSystem(void) = default;

	/// @brief 速度を持つすべてのエンティティの座標を一括更新する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief エンティティの速度を計算し、座標を更新する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void UpdatePosition(EcsRegistry& registry, Entity entity);

	/// @brief 指定したエンティティに力を加える
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param force 加える力
	void AddForce(EcsRegistry& registry, Entity entity, const VECTOR& force);

	/// @brief 速度を直接設定する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param velocity 設定する速度ベクトル
	void SetVelocity(EcsRegistry& registry, Entity entity, const VECTOR& velocity);
};

