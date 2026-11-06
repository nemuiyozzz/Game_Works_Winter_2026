#include "PlayerControlSystem.h"
#include "PlayerState/PlayerActionFacade.h"
#include "PlayerState/PlayerIdleState.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/PlayerStateComponent.h"
#include "../Component/VelocityComponent.h"
#include "../Component/TransformComponent.h"
#include "../../Engine/System/Time/TimeManager.h"
#include "PlayerState/PlayerDamageState.h"
#include "../Component/CollisionComponent.h"
#include "../Component/StatusComponent.h"
#include "CollisionSystem.h"
#include "../../Scene/MainScene/SceneResult.h"
#include "../../Scene/SceneManager.h"

void PlayerControlSystem::Update(EcsRegistry& registry)
{
	std::vector<Entity> entities =
		registry.GetEntitiesWith<PlayerInputComponent, VelocityComponent>(); 

	for (Entity entity : entities)
	{
		UpdatePlayerControl(registry, entity);
	}
}

void PlayerControlSystem::UpdatePlayerControl(EcsRegistry& registry, Entity entity)
{
	if (!registry.HasComponent<PlayerInputComponent>(entity) ||
		!registry.HasComponent<VelocityComponent>(entity) ||
		!registry.HasComponent<TransformComponent>(entity))
	{
		return;
	}

	if (!registry.HasComponent<PlayerStateComponent>(entity))
	{
		registry.AddComponent<PlayerStateComponent>(entity, PlayerStateComponent{});
	}

	PlayerStateComponent& stateComponent =
		registry.GetComponent<PlayerStateComponent>(entity);

	// サブシステム操作用のFacade窓口を生成する
	PlayerActionFacade actionFacade(registry, entity);

	// 初回フレーム時は待機ステートで初期化する
	if (stateComponent.currentState == nullptr)
	{
		stateComponent.currentState = std::make_shared<PlayerIdleState>();
		stateComponent.stateTimer = 0.0f;
		stateComponent.currentState->EnterState(actionFacade);
	}

	stateComponent.stateTimer += TimeManager::GetInstance().GetDeltaTime();

	// 現在ステートの更新と遷移判定
	std::shared_ptr<IPlayerState> nextState =
		stateComponent.currentState->UpdateState(actionFacade, TimeManager::GetInstance().GetDeltaTime());

	if (nextState != nullptr)
	{
		stateComponent.currentState->ExitState(actionFacade);
		stateComponent.currentState = nextState;
		stateComponent.stateTimer = 0.0f;
		stateComponent.currentState->EnterState(actionFacade);
	}

	UpdateStaminaRecovery(registry, entity, TimeManager::GetInstance().GetDeltaTime());

	CheckDamage(registry, entity);
}

void PlayerControlSystem::UpdateStaminaRecovery(
	EcsRegistry& registry, Entity entity, float deltaTime)
{
	if (!registry.HasComponent<PlayerStateComponent>(entity))
	{
		return;
	}

	PlayerStateComponent& stateComponent =
		registry.GetComponent<PlayerStateComponent>(entity);

	if (stateComponent.staminaRecoveryDelayTimer > 0.0f)
	{
		stateComponent.staminaRecoveryDelayTimer -= deltaTime;
		return;
	}

	if (stateComponent.stamina < stateComponent.maxStamina)
	{
		stateComponent.stamina += stateComponent.staminaRecoveryRate * deltaTime;
		if (stateComponent.stamina > stateComponent.maxStamina)
		{
			stateComponent.stamina = stateComponent.maxStamina;
		}
	}
}

void PlayerControlSystem::CheckDamage(EcsRegistry& registry, Entity playerEntity)
{
	if (registry.HasComponent<StatusComponent>(playerEntity))
	{
		auto& status = registry.GetComponent<StatusComponent>(playerEntity);
		if (status.invincibleTimer > 0.0f)
		{
			return;
		}
	}

	bool isHitEnemy = CollisionSystem::CheckHitByTag(registry, playerEntity, ColliderBase::TAG::ENEMY);

	if (isHitEnemy)
	{
		if (registry.HasComponent<StatusComponent>(playerEntity))
		{
			auto& status = registry.GetComponent<StatusComponent>(playerEntity);
			status.currentHp -= 10;

			if (status.currentHp <= 0)
			{
				status.currentHp = 0;

				SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>());
				return;
			}
		}

		if (registry.HasComponent<PlayerStateComponent>(playerEntity))
		{
			auto& stateComp = registry.GetComponent<PlayerStateComponent>(playerEntity);
			PlayerActionFacade facade(registry, playerEntity);

			// 現在のステートの終了処理を呼ぶ
			if (stateComp.currentState != nullptr)
			{
				stateComp.currentState->ExitState(facade);
			}

			stateComp.currentState = std::make_shared<PlayerDamageState>();
			stateComp.currentState->EnterState(facade);
		}
	}
}