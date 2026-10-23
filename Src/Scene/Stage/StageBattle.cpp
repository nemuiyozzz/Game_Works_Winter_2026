#include "../../Pch.h"
#include "StageBattle.h"
#include "../../Application.h"
#include "../../Ecs/ActorFactory.h"
#include "../../Ecs/Component/TransformComponent.h"

namespace
{
	// 中心座標と半径から、円周内のランダムなX-Z座標を算出する
	VECTOR GetRandomPositionInCircle(const VECTOR& center, float radius)
	{
		float randR = static_cast<float>(GetRand(10000)) / 10000.0f;
		float randTheta = static_cast<float>(GetRand(10000)) / 10000.0f;

		float r = radius * std::sqrt(randR);
		float theta = randTheta * DX_PI_F * 2.0f;

		return VAdd(center, VGet(r * std::cos(theta), 0.0f, r * std::sin(theta)));
	}
}

StageBattle::StageBattle(EcsRegistry& registry)
	: StageBase(registry)
{
}

void StageBattle::Load(void)
{
	LoadStageDataFile(
		std::wstring(Application::PATH_CSV) + L"StageBattle.csv",
		std::wstring(Application::PATH_BINARY) + L"StageBattle.bin");
}

void StageBattle::Initialize(void)
{
	StageBase::Initialize();

	constexpr int ENEMY_SPAWN_COUNT = 10;
	constexpr float SPAWN_RADIUS = 50.0f;
	VECTOR spawnCenter = VGet(1000.0f, 250.0f, -1000.0f);

	for (int i = 0; i < ENEMY_SPAWN_COUNT; ++i)
	{
		Entity enemy = ActorFactory::CreateActor(ecsRegistry_, L"Enemy");

		if (enemy != NULL_ENTITY && ecsRegistry_.HasComponent<TransformComponent>(enemy))
		{
			TransformComponent& transform = ecsRegistry_.GetComponent<TransformComponent>(enemy);
			VECTOR randomPos = GetRandomPositionInCircle(spawnCenter, SPAWN_RADIUS);

			transform.position = randomPos;
			transform.localPosition = randomPos;
			transform.previousPosition = randomPos;
		}
	}
}

void StageBattle::Update(void)
{
}

void StageBattle::Draw(void)
{
#ifdef _DEBUG
	// デバッグ表示関連
	constexpr int TEXT_POSITION_X = 20;  // テキストのX座標
	constexpr int TEXT_POSITION_Y = 220; // テキストのY座標
	constexpr int TEXT_COLOR = 0xff4444; // テキストの色

	DrawString(TEXT_POSITION_X, TEXT_POSITION_Y, L"CURRENT STAGE : BATTLE", TEXT_COLOR);
#endif // _DEBUG
}

VECTOR StageBattle::GetPlayerSpawnPosition(void) const
{
	return stageData_.playerSpawnPosition;
}