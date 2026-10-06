#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief プレイヤー入力を受け取り、エンティティを操作するシステムクラス
class PlayerControlSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	PlayerControlSystem(void) = default;

	/// @brief デストラクタ
	~PlayerControlSystem(void) = default;

	/// @brief プレイヤー入力コンポーネントを持つすべてのエンティティを操作する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief プレイヤー入力に応じてエンティティの速度を変更する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void UpdatePlayerControl(EcsRegistry& registry, Entity entity);
};

