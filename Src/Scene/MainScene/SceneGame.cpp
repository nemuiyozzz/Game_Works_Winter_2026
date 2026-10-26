#include "../../Pch.h"
#include "SceneGame.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../../Ecs/Component/EnemyComponent.h"
#include "../../Ecs/Component/CollisionComponent.h"
#include "../Stage/StageLobby.h"
#include "../Stage/StageBattle.h"
#include "../Stage/StageTraining.h"
#include "SceneResult.h"
#include "../SceneManager.h"
#include "../../Ecs/Component/HierarchyComponent.h"
#include "../../Ecs/System/CollisionSystem.h"

namespace
{	
	// リソース・レシピキー名関連
	constexpr const wchar_t* KEY_MODEL_PLAYER = L"MODEL_PLAYER"; // プレイヤーモデル
	constexpr const wchar_t* RECIPE_PLAYER = L"Player";          // プレイヤーレシピ名
	constexpr const wchar_t* KEY_MODEL_ENEMY = L"MODEL_SAMPLE"; // エネミーモデル
	constexpr const wchar_t* KEY_MODEL_SWORD = L"MODEL_SWORD"; // 剣モデル
}

SceneGame::SceneGame(void)
	: playerEntity_(NULL_ENTITY)
	, cameraEntity_(NULL_ENTITY)
	, defeatedEnemyCount_(0)
{
}

SceneGame::~SceneGame(void)
{
}

void SceneGame::Load(void)
{
	SceneBase::Load();

	ResourceManager::GetInstance().Load(KEY_MODEL_PLAYER);
	ResourceManager::GetInstance().Load(KEY_MODEL_ENEMY);
	ResourceManager::GetInstance().Load(KEY_MODEL_SWORD);

	ResourceManager::GetInstance().Load(L"ANIM_PLAYER_RUN");
	ResourceManager::GetInstance().Load(L"ANIM_PLAYER_JUMP");
	ResourceManager::GetInstance().Load(L"ANIM_PLAYER_ATTACK");
}

void SceneGame::Initialize(void)
{
	SceneBase::Initialize();
	ecsWorld_.Initialize();

	playerEntity_ = ecsWorld_.SpawnActor(RECIPE_PLAYER);
	cameraEntity_ = ecsWorld_.SpawnCamera(playerEntity_);
	Entity swordEntity = ecsWorld_.SpawnActor(L"Sword");
	if (swordEntity != NULL_ENTITY && ecsWorld_.GetRegistry().HasComponent<HierarchyComponent>(swordEntity))
	{
		ecsWorld_.GetRegistry().GetComponent<HierarchyComponent>(swordEntity).parent = playerEntity_;
	}

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

	auto enemies = ecsWorld_.GetRegistry().GetEntitiesWith<EnemyComponent, CollisionComponent>();

	for (Entity enemy : enemies)
	{
		if (CollisionSystem::CheckHitByTag(ecsWorld_.GetRegistry(), enemy,
			ColliderBase::TAG::PLAYER_SWORD))
		{
			ecsWorld_.GetRegistry().DestroyEntity(enemy);
			defeatedEnemyCount_++;
		}
	}

	// 5体倒したらリザルトシーンへ遷移
	if (defeatedEnemyCount_ >= 5)
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneResult>());
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