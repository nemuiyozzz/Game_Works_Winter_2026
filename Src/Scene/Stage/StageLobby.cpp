#include "../../Pch.h"
#include "StageLobby.h"
#include "../../Application.h"

StageLobby::StageLobby(EcsRegistry& registry)
	: StageBase(registry)
{
}

void StageLobby::Load(void)
{
	LoadStageDataFile(
		std::wstring(Application::PATH_CSV) + L"StageLobby.csv",
		std::wstring(Application::PATH_BINARY) + L"StageLobby.bin");
}

void StageLobby::Initialize(void)
{
}

void StageLobby::Update(void)
{
}

void StageLobby::Draw(void)
{
#ifdef _DEBUG
	// デバッグ表示関連
	constexpr int TEXT_POSITION_X = 20;      // テキストのX座標
	constexpr int TEXT_POSITION_Y = 220;     // テキストのY座標
	constexpr int TEXT_COLOR = 0x00ff00;     // テキストの色（緑）

	DrawString(TEXT_POSITION_X, TEXT_POSITION_Y, L"CURRENT STAGE : LOBBY", TEXT_COLOR);
#endif // _DEBUG
}

VECTOR StageLobby::GetPlayerSpawnPosition(void) const
{
	return SPAWN_POSITION;
}