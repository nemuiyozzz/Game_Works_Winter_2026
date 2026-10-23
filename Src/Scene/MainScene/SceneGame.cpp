#include "../../Pch.h"
#include "SceneGame.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../Stage/StageLobby.h"
#include "../Stage/StageBattle.h"
#include "../Stage/StageTraining.h"

namespace
{
	// リソース・レシピキー名関連
	constexpr const wchar_t* KEY_MODEL_PLAYER = L"MODEL_SAMPLE"; // プレイヤーモデルキー
	constexpr const wchar_t* RECIPE_PLAYER = L"Player";          // プレイヤーレシピ名
}

SceneGame::SceneGame(void)
	: playerEntity_(NULL_ENTITY)
	, cameraEntity_(NULL_ENTITY)
{
}

SceneGame::~SceneGame(void)
{
}

void SceneGame::Load(void)
{
	SceneBase::Load();

	ResourceManager::GetInstance().Load(KEY_MODEL_PLAYER);
}

void SceneGame::Initialize(void)
{
	SceneBase::Initialize();
	ecsWorld_.Initialize();

	playerEntity_ = ecsWorld_.SpawnActor(RECIPE_PLAYER);
	cameraEntity_ = ecsWorld_.SpawnCamera(playerEntity_);

	EcsRegistry& registry = ecsWorld_.GetRegistry();
	stageManager_ = std::make_unique<StageManager>(registry, playerEntity_);
	stageManager_->Initialize();
	stageManager_->SetupFirstStage(std::make_shared<StageBattle>(registry));
}

void SceneGame::Update(void)
{
	stageManager_->Update();

	if (stageManager_->GetStageChanging())
	{
		return;
	}

#ifdef _DEBUG
	EcsRegistry& registry = ecsWorld_.GetRegistry();

	if (CheckHitKey(KEY_INPUT_F5) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageLobby>(registry));
	}
	else if (CheckHitKey(KEY_INPUT_F6) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageBattle>(registry));
	}
	else if (CheckHitKey(KEY_INPUT_F7) != 0)
	{
		stageManager_->ChangeStage(std::make_shared<StageTraining>(registry));
	}
#endif // _DEBUG

	ecsWorld_.Update();
}

void SceneGame::Draw(void)
{
	stageManager_->DrawStage();

	ecsWorld_.Draw();

	stageManager_->DrawOverlay();
}

void SceneGame::Release(void)
{
	if (stageManager_ != nullptr)
	{
		stageManager_->Release();
	}

	ecsWorld_.Release();
}