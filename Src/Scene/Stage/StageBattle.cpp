#include "../../Pch.h"
#include "StageBattle.h"
#include "../../Application.h"

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