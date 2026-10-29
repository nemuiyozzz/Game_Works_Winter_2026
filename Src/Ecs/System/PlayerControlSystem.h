#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief プレイヤー入力を受け取り、ステートパターンとFacadeを介してエンティティを操作するシステムクラス
class PlayerControlSystem : public ISystem
{
public:

	// プレイヤーのアニメーション種別
	enum class PLAYER_ANIM
	{
		IDLE,
		MOVE,
		JUMP,
		ATTACK
	};

	/// @brief コンストラクタ 
	PlayerControlSystem(void) = default;

	/// @brief デストラクタ
	~PlayerControlSystem(void) = default;

	/// @brief プレイヤー入力コンポーネントを持つすべてのエンティティを操作する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief プレイヤーのステート更新とスタミナ回復を行う
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void UpdatePlayerControl(EcsRegistry& registry, Entity entity);

private:

	/// @brief スタミナの自然回復処理を行う
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param deltaTime フレーム経過時間
	void UpdateStaminaRecovery(EcsRegistry& registry, Entity entity, float deltaTime);
};