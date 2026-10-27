#include "../../Pch.h"
#include "StatusSystem.h"
#include "../Component/StatusComponent.h"
#include "../../Engine/System/Time/TimeManager.h"

namespace
{
	// ダメージ・レベルアップ計算関連
	constexpr int MIN_DAMAGE = 1;                  // 最低保証ダメージ
	constexpr float NEXT_EXP_MULTIPLIER = 1.5f;    // レベルアップ毎の必要経験値増加倍率
}

void StatusSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<StatusComponent>();
	float deltaTime = TimeManager::GetInstance().GetDeltaTime();

	for (Entity entity : entities)
	{
		StatusComponent& status = registry.GetComponent<StatusComponent>(entity);

		if (status.invincibleTimer > 0.0f)
		{
			status.invincibleTimer -= deltaTime;
			if (status.invincibleTimer < 0.0f)
			{
				status.invincibleTimer = 0.0f;
			}
		}
	}
}

bool StatusSystem::ApplyDamage(
	EcsRegistry& registry, Entity attackerEntity, Entity targetEntity)
{
	if (!registry.HasComponent<StatusComponent>(attackerEntity) ||
		!registry.HasComponent<StatusComponent>(targetEntity))
	{
		return false;
	}

	StatusComponent& targetStatus = registry.GetComponent<StatusComponent>(targetEntity);

	// 無敵時間中、または既にHPが0の場合はダメージを与えない
	if (targetStatus.invincibleTimer > 0.0f || targetStatus.currentHp <= 0)
	{
		return false;
	}

	const StatusComponent& attackerStatus =
		registry.GetComponent<StatusComponent>(attackerEntity);

	int damage = attackerStatus.attackPower - targetStatus.defensePower;
	if (damage < MIN_DAMAGE)
	{
		damage = MIN_DAMAGE;
	}

	targetStatus.currentHp -= damage;
	targetStatus.invincibleTimer = targetStatus.invincibleDuration;

	if (targetStatus.currentHp <= 0)
	{
		targetStatus.currentHp = 0;
		return true;
	}

	return false;
}

void StatusSystem::AddExperience(
	EcsRegistry& registry, Entity targetEntity, int experienceAmount)
{
	if (!registry.HasComponent<StatusComponent>(targetEntity))
	{
		return;
	}

	// 経験値を加算するだけ（ここではレベルアップさせない）
	registry.GetComponent<StatusComponent>(targetEntity).currentExperience += experienceAmount;
}

bool StatusSystem::TryLevelUp(EcsRegistry& registry, Entity targetEntity)
{
	if (!registry.HasComponent<StatusComponent>(targetEntity))
	{
		return false;
	}

	StatusComponent& status = registry.GetComponent<StatusComponent>(targetEntity);

	// 経験値が足りなければ失敗
	if (status.currentExperience < status.nextLevelExperience)
	{
		return false;
	}

	// 経験値を消費してレベルアップ
	status.currentExperience -= status.nextLevelExperience;
	status.level++;

	status.maxHp += status.growthHp;
	status.currentHp = status.maxHp;
	status.attackPower += status.growthAttack;
	status.defensePower += status.growthDefense;

	// 次の必要経験値を増加
	constexpr float NEXT_EXP_MULTIPLIER = 1.5f;
	
	status.nextLevelExperience = static_cast<int>(
		static_cast<float>(status.nextLevelExperience) * NEXT_EXP_MULTIPLIER);

	return true;
}