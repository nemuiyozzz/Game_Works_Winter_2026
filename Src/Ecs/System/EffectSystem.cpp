#include "../../Pch.h"
#include "EffectSystem.h"
#include <EffekseerForDXLib.h>
#include "../Component/EffectComponent.h"
#include "../Component/TransformComponent.h"
#include "../../Engine/System/Resource/ResourceManager.h"

void EffectSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<EffectComponent, TransformComponent>();

	for (Entity entity : entities)
	{
		EffectComponent& effect = registry.GetComponent<EffectComponent>(entity);
		const TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);

		if (effect.resourceKey.empty())
		{
			continue;
		}

		bool isCurrentlyPlaying = (effect.playHandle != -1) &&
			(IsEffekseer3DEffectPlaying(effect.playHandle) == 0);

		// 再生リクエストがある、またはループ設定で再生が切れた場合に新規再生
		if (effect.isPlayRequested || (!isCurrentlyPlaying && effect.isLoop))
		{
			if (isCurrentlyPlaying)
			{
				StopEffekseer3DEffect(effect.playHandle);
			}

			int resourceHandle = ResourceManager::GetInstance().GetHandleId(effect.resourceKey);
			if (resourceHandle != -1)
			{
				effect.playHandle = PlayEffekseer3DEffect(resourceHandle);
				isCurrentlyPlaying = (effect.playHandle != -1);
			}

			effect.isPlayRequested = false;
		}

		// 再生中の場合は座標・回転・スケール・速度を同期
		if (isCurrentlyPlaying && effect.isFollow)
		{
			VECTOR worldPos = VAdd(transform.position, effect.localOffset);
			VECTOR euler = transform.rotation_.ToEuler();

			SetPosPlayingEffekseer3DEffect(
				effect.playHandle, worldPos.x, worldPos.y, worldPos.z);
			SetRotationPlayingEffekseer3DEffect(
				effect.playHandle, euler.x, euler.y, euler.z);
			SetScalePlayingEffekseer3DEffect(
				effect.playHandle, effect.scale.x, effect.scale.y, effect.scale.z);
			SetSpeedPlayingEffekseer3DEffect(
				effect.playHandle, effect.speed);
		}
	}

	// DXライブラリの3Dカメラ設定とEffekseerを同期して全体を更新
	Effekseer_Sync3DSetting();
	UpdateEffekseer3D();
}

void EffectSystem::Draw(void)
{
	DrawEffekseer3D();
}

void EffectSystem::RequestPlayEffect(
	EcsRegistry& registry,
	Entity targetEntity,
	const std::wstring& resourceKey,
	const VECTOR& scale,
	float speed)
{
	if (!registry.HasComponent<EffectComponent>(targetEntity))
	{
		registry.AddComponent<EffectComponent>(targetEntity, EffectComponent{});
	}

	EffectComponent& effect = registry.GetComponent<EffectComponent>(targetEntity);
	effect.resourceKey = resourceKey;
	effect.scale = scale;
	effect.speed = speed;
	effect.isLoop = false;
	effect.isPlayRequested = true;
}