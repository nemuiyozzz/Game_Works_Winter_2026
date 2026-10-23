#include "../../Pch.h"
#include "StageManager.h"
#include "StageBase.h"
#include "../../Common/Fader.h"
#include "../../Common/Loading.h"
#include "../../Ecs/Component/TransformComponent.h"
#include "../../Ecs/Component/VelocityComponent.h"
#include "../../Engine/Utility/UtilityMath.h"

namespace
{
	constexpr int LOAD_COMPLETE_THRESHOLD = 100;
}

StageManager::StageManager(EcsRegistry& registry, Entity playerEntity)
	: ecsRegistry_(registry)
	, playerEntity_(playerEntity)
	, currentStage_(nullptr)
	, nextStage_(nullptr)
	, oldStage_(nullptr)
	, isStageChangeing_(false)
{
	fader_ = std::make_unique<Fader>();
	loading_ = std::make_unique<Loading>();
}

StageManager::~StageManager(void)
{
	Release();
}

void StageManager::Initialize(void)
{
	fader_->Initialize();
	loading_->Initialize();
}

void StageManager::Update(void)
{
	if (isStageChangeing_)
	{
		fader_->Update();
		loading_->Update();

		const bool isLoadFinished = 
			(loading_->GetProgress() >= LOAD_COMPLETE_THRESHOLD && !loading_->GetLoading());
		const bool isFaderOutFinished =
			(fader_->GetState() == Fader::FADE_STATE::FADE_OUT && fader_->GetEnd());

		if (isLoadFinished && isFaderOutFinished && nextStage_ != nullptr)
		{
			if (currentStage_ != nullptr)
			{
				oldStage_ = currentStage_;
				oldStage_->Release();
				oldStage_ = nullptr;
			}

			nextStage_->EndLoad();
			nextStage_->Initialize();
			currentStage_ = nextStage_;
			nextStage_ = nullptr;

			ResetPlayerTransform();

			fader_->SetFade(Fader::FADE_STATE::FADE_IN);
		}

		if (nextStage_ == nullptr &&
			fader_->GetState() == Fader::FADE_STATE::FADE_IN &&
			fader_->GetEnd())
		{
			fader_->SetFade(Fader::FADE_STATE::NONE);
			isStageChangeing_ = false;
		}

		return;
	}

	if (currentStage_ != nullptr)
	{
		currentStage_->Update();
	}
}

void StageManager::DrawStage(void)
{
	if (currentStage_ != nullptr)
	{
		currentStage_->Draw();
	}
}

void StageManager::DrawOverlay(void)
{
	fader_->Draw();

	if (isStageChangeing_ &&
		fader_->GetState() == Fader::FADE_STATE::FADE_OUT &&
		fader_->GetEnd())
	{
		loading_->Draw();
	}
}

void StageManager::Release(void)
{
	if (loading_->GetLoading())
	{
		loading_->EndAsyncLoad();
	}

	if (currentStage_ != nullptr)
	{
		currentStage_->Release();
		currentStage_ = nullptr;
	}

	nextStage_ = nullptr;
	oldStage_ = nullptr;
}

void StageManager::ChangeStage(std::shared_ptr<StageBase> nextStage)
{
	if (isStageChangeing_ || nextStage == nullptr)
	{
		return;
	}

	nextStage_ = nextStage;
	isStageChangeing_ = true;

	fader_->SetFade(Fader::FADE_STATE::FADE_OUT);

	loading_->StartAsyncLoad([nextStage]()
		{
			nextStage->Load();
		});
}

void StageManager::SetupFirstStage(std::shared_ptr<StageBase> firstStage)
{
	if (firstStage == nullptr) { return; }

	currentStage_ = firstStage;
	currentStage_->Load();
	currentStage_->EndLoad();
	currentStage_->Initialize();

	ResetPlayerTransform();
}

void StageManager::ResetPlayerTransform(void)
{
	if (playerEntity_ == NULL_ENTITY || currentStage_ == nullptr)
	{
		return;
	}

	if (ecsRegistry_.HasComponent<TransformComponent>(playerEntity_))
	{
		TransformComponent& playerTransform =
			ecsRegistry_.GetComponent<TransformComponent>(playerEntity_);

		VECTOR spawnPosition = currentStage_->GetPlayerSpawnPosition();
		playerTransform.position = spawnPosition;
		playerTransform.localPosition = spawnPosition;
		playerTransform.previousPosition = spawnPosition;
	}

	if (ecsRegistry_.HasComponent<VelocityComponent>(playerEntity_))
	{
		VelocityComponent& playerVelocity =
			ecsRegistry_.GetComponent<VelocityComponent>(playerEntity_);

		playerVelocity.velocity = UtilityMath::VECTOR_ZERO;
		playerVelocity.acceleration = UtilityMath::VECTOR_ZERO;
	}
}