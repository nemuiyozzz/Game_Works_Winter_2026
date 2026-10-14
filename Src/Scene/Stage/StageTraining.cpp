#include "../../Pch.h"
#include "StageTraining.h"

StageTraining::StageTraining(EcsRegistry& registry)
	: StageBase(registry)
{
}

void StageTraining::Load(void)
{
}

void StageTraining::Initialize(void)
{
}

void StageTraining::Update(void)
{
}

void StageTraining::Draw(void)
{
#ifdef _DEBUG

	// デバッグ表示関連
	constexpr int TEXT_POSITION_X = 20;      // テキストのX座標
	constexpr int TEXT_POSITION_Y = 220;     // テキストのY座標
	constexpr int TEXT_COLOR = 0x00ffff;     // テキストの色（水色）

	DrawString(TEXT_POSITION_X, TEXT_POSITION_Y, L"CURRENT STAGE : TRAINING", TEXT_COLOR);

#endif // _DEBUG
}

VECTOR StageTraining::GetPlayerSpawnPosition(void) const
{
	return SPAWN_POSITION;
}