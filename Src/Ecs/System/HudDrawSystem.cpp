#include "../../Pch.h"
#include "HudDrawSystem.h"
#include "../Component/PlayerInputComponent.h"
#include "../Component/StatusComponent.h"
#include "../Component/PlayerStateComponent.h"

namespace
{
	// HUD描画用の定数
	constexpr int BAR_BASE_X = 50;      // バーの左上基準X座標
	constexpr int HP_BAR_Y = 50;        // HPバーのY座標
	constexpr int STAMINA_BAR_Y = 85;   // スタミナバーのY座標
	constexpr int BAR_MAX_WIDTH = 350;  // バーの最大幅（HP・スタミナ共通）
	constexpr int BAR_HEIGHT = 15;      // バーの高さ

	// 色の定義 (DxLibのカラーコード)
	constexpr unsigned int COLOR_BG = 0x333333;      // 背景色（暗いグレー）
	constexpr unsigned int COLOR_HP = 0xff0000;      // HPの色（赤色）
	constexpr unsigned int COLOR_STAMINA = 0x33cc44; // スタミナの色（緑色）
	constexpr unsigned int COLOR_FRAME = 0xdddddd;   // 枠線の色（白灰）
}

void HudDrawSystem::Update(EcsRegistry& registry)
{
	// プレイヤーエンティティを取得する
	std::vector<Entity> players =
		registry.GetEntitiesWith<PlayerInputComponent, StatusComponent, PlayerStateComponent>();

	if (players.empty())
	{
		return;
	}

	Entity playerEntity = players[0];
	const StatusComponent& status = registry.GetComponent<StatusComponent>(playerEntity);
	const PlayerStateComponent& state = registry.GetComponent<PlayerStateComponent>(playerEntity);

	float hpRatio = static_cast<float>(status.currentHp) / static_cast<float>(status.maxHp);
	if (hpRatio < 0.0f) hpRatio = 0.0f;
	if (hpRatio > 1.0f) hpRatio = 1.0f;
	int currentHpWidth = static_cast<int>(BAR_MAX_WIDTH * hpRatio);

	float staminaRatio = state.stamina / state.maxStamina;
	if (staminaRatio < 0.0f) staminaRatio = 0.0f;
	if (staminaRatio > 1.0f) staminaRatio = 1.0f;
	int currentStaminaWidth = static_cast<int>(BAR_MAX_WIDTH * staminaRatio);

	// 背景（ゲージが減った部分）
	DrawBox(BAR_BASE_X, HP_BAR_Y, BAR_BASE_X + BAR_MAX_WIDTH, HP_BAR_Y + BAR_HEIGHT, COLOR_BG, true);
	// 現在のHP
	if (currentHpWidth > 0)
	{
		DrawBox(BAR_BASE_X, HP_BAR_Y, BAR_BASE_X + currentHpWidth, HP_BAR_Y + BAR_HEIGHT, COLOR_HP, true);
	}
	// 枠線
	DrawBox(BAR_BASE_X, HP_BAR_Y, BAR_BASE_X + BAR_MAX_WIDTH, HP_BAR_Y + BAR_HEIGHT, COLOR_FRAME, false);

	// 背景
	DrawBox(BAR_BASE_X, STAMINA_BAR_Y, BAR_BASE_X + BAR_MAX_WIDTH, STAMINA_BAR_Y + BAR_HEIGHT, COLOR_BG, true);
	
	// 現在のスタミナ
	if (currentStaminaWidth > 0)
	{
		DrawBox(BAR_BASE_X, STAMINA_BAR_Y, BAR_BASE_X + currentStaminaWidth, STAMINA_BAR_Y + BAR_HEIGHT, COLOR_STAMINA, true);
	}
	// 枠線
	DrawBox(BAR_BASE_X, STAMINA_BAR_Y, BAR_BASE_X + BAR_MAX_WIDTH, STAMINA_BAR_Y + BAR_HEIGHT, COLOR_FRAME, false);
}