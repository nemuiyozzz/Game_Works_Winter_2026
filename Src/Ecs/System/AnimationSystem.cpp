#include "../../Pch.h"
#include "AnimationSystem.h"
#include "../Component/AnimationComponent.h"
#include "../Component/ModelComponent.h"
#include "../../Engine/System/Time/TimeManager.h"

void AnimationSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<AnimationComponent, ModelComponent>();

	for (Entity entity : entities)
	{
		UpdateAnimation(registry, entity);
	}
}

void AnimationSystem::UpdateAnimation(EcsRegistry& registry, Entity entity)
{
	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);
	ModelComponent& modelComponent = registry.GetComponent<ModelComponent>(entity);

	// 一時停止中の処理
	if (animationComp.isPaused)
	{
		if (IsValidType(animationComp, animationComp.playType))
		{
			auto& animationData = animationComp.animations[animationComp.playType];
			MV1SetAttachAnimTime(modelComponent.modelHandle,
				animationData.attachNumber, animationData.currentStep);
		}

		return;
	}

	float deltaTime = TimeManager::GetInstance().GetDeltaTime();

	// 再生中アニメーションの進行
	if (IsValidType(animationComp, animationComp.playType))
	{
		auto& animationData = animationComp.animations[animationComp.playType];

		animationData.currentStep += deltaTime * animationData.playSpeed;

		if (animationComp.isLoop && animationData.currentStep >= animationData.totalTime)
		{
			animationData.currentStep = 0.0f;
		}

		MV1SetAttachAnimTime(modelComponent.modelHandle,
			animationData.attachNumber, animationData.currentStep);
	}

	// ブレンド（フェード遷移）処理
	if (IsValidType(animationComp, animationComp.previousType))
	{
		animationComp.blendTimer += deltaTime;

		float blendRate = 1.0f;

		if (animationComp.blendTime > 0.0f)
		{
			blendRate = animationComp.blendTimer / animationComp.blendTime;
		}

		if (blendRate > 1.0f)
		{
			blendRate = 1.0f;
		}

		auto& previousAnimation = animationComp.animations[animationComp.previousType];
		auto& currentAnimation = animationComp.animations[animationComp.playType];

		MV1SetAttachAnimBlendRate(modelComponent.modelHandle,
			previousAnimation.attachNumber, 1.0f - blendRate);

		MV1SetAttachAnimBlendRate(modelComponent.modelHandle,
			currentAnimation.attachNumber, blendRate);

		// ブレンド完了
		if (blendRate >= 1.0f)
		{
			MV1DetachAnim(modelComponent.modelHandle, previousAnimation.attachNumber);
			previousAnimation.attachNumber = -1;
			animationComp.previousType = -1;

			MV1SetAttachAnimBlendRate(modelComponent.modelHandle,
				currentAnimation.attachNumber, 1.0f);
		}
	}
}

void AnimationSystem::AddInternal(EcsRegistry& registry, Entity entity, int type,
	int animationIndex, float speed)
{
	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (type >= animationComp.animations.size())
	{
		animationComp.animations.resize(type + 1);
	}

	AnimData animationData{};
	animationData.isRegistered = true;
	animationData.modelHandle = -1;
	animationData.animationIndex = animationIndex;
	animationData.playSpeed = speed;
	animationData.mode = ANIM_MODE::INTERNAL;

	animationComp.animations[type] = animationData;
}

void AnimationSystem::AddExternal(EcsRegistry& registry, Entity entity, int type,
	int modelHandle, float speed)
{
	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (type >= animationComp.animations.size())
	{
		animationComp.animations.resize(type + 1);
	}

	AnimData animationData{};
	animationData.isRegistered = true;
	animationData.modelHandle = modelHandle;
	animationData.animationIndex = 0;
	animationData.playSpeed = speed;
	animationData.mode = ANIM_MODE::EXTERNAL;

	animationComp.animations[type] = animationData;
}

void AnimationSystem::Play(EcsRegistry& registry, Entity entity, int type,
	bool isLoop, float blendTime)
{
	if (!registry.HasComponent<AnimationComponent>(entity) ||
		!registry.HasComponent<ModelComponent>(entity))
	{
		return;
	}

	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);
	ModelComponent& modelComponent = registry.GetComponent<ModelComponent>(entity);

	if (type == animationComp.playType)
	{
		return;
	}

	if (!IsValidType(animationComp, type))
	{
		return;
	}

	if (IsValidType(animationComp, animationComp.previousType))
	{
		auto& previousAnimation = animationComp.animations[animationComp.previousType];

		MV1DetachAnim(modelComponent.modelHandle, previousAnimation.attachNumber);
		previousAnimation.attachNumber = -1;
		animationComp.previousType = -1;
	}

	if (IsValidType(animationComp, animationComp.playType))
	{
		animationComp.previousType = animationComp.playType;
	}

	animationComp.playType = type;
	animationComp.isLoop = isLoop;
	animationComp.blendTime = blendTime;
	animationComp.blendTimer = 0.0f;
	animationComp.isPaused = false;

	auto& animationData = animationComp.animations[type];
	animationData.currentStep = 0.0f;

	if (animationData.mode == ANIM_MODE::INTERNAL)
	{
		animationData.attachNumber = MV1AttachAnim(modelComponent.modelHandle,
			animationData.animationIndex);
	}
	else
	{
		animationData.attachNumber = MV1AttachAnim(modelComponent.modelHandle,
			animationData.animationIndex, animationData.modelHandle);
	}

	animationData.totalTime = MV1GetAttachAnimTotalTime(
		modelComponent.modelHandle, animationData.attachNumber);

	if (animationComp.previousType == -1)
	{
		MV1SetAttachAnimBlendRate(modelComponent.modelHandle,
			animationData.attachNumber, 1.0f);
	}
	else
	{
		MV1SetAttachAnimBlendRate(modelComponent.modelHandle,
			animationData.attachNumber, 0.0f);
	}
}

bool AnimationSystem::IsEnd(EcsRegistry& registry, Entity entity)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (animationComp.playType == -1 || animationComp.isLoop)
	{
		return false;
	}

	if (!IsValidType(animationComp, animationComp.playType))
	{
		return false;
	}

	return animationComp.animations[animationComp.playType].currentStep >=
		animationComp.animations[animationComp.playType].totalTime;
}

bool AnimationSystem::IsPlaying(EcsRegistry& registry, Entity entity, int type)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (animationComp.playType != type)
	{
		return false;
	}

	if (animationComp.isLoop)
	{
		return true;
	}

	if (!IsValidType(animationComp, type))
	{
		return false;
	}

	return animationComp.animations[type].currentStep <
		animationComp.animations[type].totalTime;
}

void AnimationSystem::Pause(EcsRegistry& registry, Entity entity)
{
	registry.GetComponent<AnimationComponent>(entity).isPaused = true;
}

void AnimationSystem::Resume(EcsRegistry& registry, Entity entity)
{
	registry.GetComponent<AnimationComponent>(entity).isPaused = false;
}

void AnimationSystem::PauseAtTime(EcsRegistry& registry, Entity entity, float time)
{
	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);
	ModelComponent& modelComponent = registry.GetComponent<ModelComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		auto& animationData = animationComp.animations[animationComp.playType];

		if (time < 0.0f)
		{
			time = 0.0f;
		}
		if (time > animationData.totalTime)
		{
			time = animationData.totalTime;
		}

		animationData.currentStep = time;
		MV1SetAttachAnimTime(modelComponent.modelHandle,
			animationData.attachNumber, animationData.currentStep);

		animationComp.isPaused = true;
	}
}

void AnimationSystem::PauseAtFrame(EcsRegistry& registry, Entity entity, int frameNumber)
{
	AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);
	ModelComponent& modelComponent = registry.GetComponent<ModelComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		auto& animationData = animationComp.animations[animationComp.playType];

		float totalFrames = animationData.totalTime * animationData.playSpeed;

		if (frameNumber < 0)
		{
			frameNumber = 0;
		}

		if (frameNumber > (int)totalFrames)
		{
			frameNumber = (int)totalFrames;
		}

		float time = (float)frameNumber / animationData.playSpeed;

		if (time > animationData.totalTime)
		{
			time = animationData.totalTime;
		}

		animationData.currentStep = time;
		MV1SetAttachAnimTime(modelComponent.modelHandle,
			animationData.attachNumber, animationData.currentStep);

		animationComp.isPaused = true;
	}
}

int AnimationSystem::GetPlayType(EcsRegistry& registry, Entity entity)
{
	return registry.GetComponent<AnimationComponent>(entity).playType;
}

int AnimationSystem::GetPlayFrame(EcsRegistry& registry, Entity entity)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		const auto& animationData = animationComp.animations[animationComp.playType];
		return (int)(animationData.currentStep * animationData.playSpeed);
	}

	return 0;
}

int AnimationSystem::GetTotalPlayFrames(EcsRegistry& registry, Entity entity)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		const auto& animationData = animationComp.animations[animationComp.playType];
		return (int)(animationData.totalTime * animationData.playSpeed);
	}

	return 0;
}

float AnimationSystem::GetPlayTime(EcsRegistry& registry, Entity entity)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		return animationComp.animations[animationComp.playType].currentStep;
	}

	return 0.0f;
}

float AnimationSystem::GetTotalPlayTime(EcsRegistry& registry, Entity entity)
{
	const AnimationComponent& animationComp = registry.GetComponent<AnimationComponent>(entity);

	if (IsValidType(animationComp, animationComp.playType))
	{
		return animationComp.animations[animationComp.playType].totalTime;
	}

	return 0.0f;
}

bool AnimationSystem::IsValidType(const AnimationComponent& animationComp, int type)
{
	if (type < 0)
	{
		return false;
	}

	if (type >= animationComp.animations.size())
	{
		return false;
	}

	return animationComp.animations[type].isRegistered;
}