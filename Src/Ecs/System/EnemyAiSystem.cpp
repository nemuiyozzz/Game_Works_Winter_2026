#include "../../Pch.h"
#include "EnemyAiSystem.h"
#include "EnemyState/EnemyActionFacade.h"
#include "../Component/EnemyComponent.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/TransformComponent.h"

namespace
{
	// 攻撃・フレーム時間関連定数
	constexpr float FIXED_DELTA_TIME = 1.0f / 60.0f; // 1フレーム経過時間
	constexpr float ATTACK_DURATION = 0.8f;          // 攻撃モーションの持続秒数
	constexpr float ATTACK_COOLDOWN = 1.5f;          // 次の攻撃までのクールダウン秒数
}

void EnemyAiSystem::Update(EcsRegistry& registry)
{
	std::vector<Entity> players =
		registry.GetEntitiesWith<PlayerInputComponent, TransformComponent>(); // プレイヤー一覧
	if (players.empty())
	{
		return;
	}

	const VECTOR& playerPosition =
		registry.GetComponent<TransformComponent>(players[0]).position; // プレイヤー座標

	std::vector<Entity> enemies =
		registry.GetEntitiesWith<EnemyComponent, TransformComponent>(); // エネミー一覧

	for (Entity enemyEntity : enemies)
	{
		UpdateSingleEnemyAi(registry, enemyEntity, playerPosition);
	}
}

void EnemyAiSystem::UpdateSingleEnemyAi(
	EcsRegistry& registry, Entity enemyEntity, const VECTOR& playerPosition)
{
	EnemyActionFacade action(registry, enemyEntity); // エネミー操作用Facade窓口

	if (action.CheckDead())
	{
		action.SetPlayerDetected(false);
		action.StopHorizontalMovement();
		return;
	}

	action.UpdateTimersAndInitializeHome(FIXED_DELTA_TIME);

	switch (action.GetCurrentState())
	{
	case EnemyComponent::AI_STATE::IDLE:
		action.StopHorizontalMovement();
		action.SetPlayerDetected(false);

		// 未発見時は前方の扇形（円錐）視野で索敵する
		if (action.CheckPlayerInConeVision(playerPosition))
		{
			action.SetPlayerDetected(true);
			action.ChangeState(EnemyComponent::AI_STATE::CHASE);
		}
		break;

	case EnemyComponent::AI_STATE::CHASE:
		// 発見後は球状（360度）範囲内にいる限り回り込まれても見失わず追跡する
		if (!action.CheckPlayerInSphericalAggro(playerPosition))
		{
			action.SetPlayerDetected(false);
			action.ChangeState(EnemyComponent::AI_STATE::RETURN);
			break;
		}

		action.SetPlayerDetected(true);

		if (action.CheckCanStartAttack(playerPosition))
		{
			action.ChangeState(EnemyComponent::AI_STATE::ATTACK);
			break;
		}

		action.MoveTowardsPlayer(playerPosition);
		break;

	case EnemyComponent::AI_STATE::ATTACK:
		action.StopHorizontalMovement();
		action.SetPlayerDetected(true);

		if (action.GetStateTimer() >= ATTACK_DURATION)
		{
			action.SetAttackCooldown(ATTACK_COOLDOWN);
			action.ChangeState(EnemyComponent::AI_STATE::CHASE);
		}
		break;

	case EnemyComponent::AI_STATE::RETURN:
		action.SetPlayerDetected(false);

		if (action.CheckPlayerInConeVision(playerPosition))
		{
			action.SetPlayerDetected(true);
			action.ChangeState(EnemyComponent::AI_STATE::CHASE);
			break;
		}

		if (action.CheckArrivedAtHome())
		{
			action.StopHorizontalMovement();
			action.ChangeState(EnemyComponent::AI_STATE::IDLE);
		}
		else
		{
			action.MoveTowardsHome();
		}
		break;
	}
}

void EnemyAiSystem::DrawDebugVision(EcsRegistry& registry)
{
#ifdef _DEBUG
	std::vector<Entity> enemies =
		registry.GetEntitiesWith<EnemyComponent, TransformComponent>(); // エネミー一覧

	for (Entity enemyEntity : enemies)
	{
		EnemyActionFacade action(registry, enemyEntity);
		action.DrawDebugVisionRange();
	}
#endif // _DEBUG
}