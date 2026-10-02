#include "../../Pch.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "../SceneManager.h"
#include "../../System/Input/KeyConfInputManager.h"
#include "../../Ecs/Component/TransformComponent.h"
#include <DxLib.h>

SceneGame::SceneGame(void)
	: testPlayerEntity_(NULL_ENTITY)
{
}

SceneGame::~SceneGame(void)
{
}

void SceneGame::Load(void)
{
	SceneBase::Load();
}

void SceneGame::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneGame::Initialize(void)
{
	SceneBase::Initialize();

	// 新しいエンティティを発行
	testPlayerEntity_ = ecsRegistry_.CreateEntity();

	// データを作成して初期値を設定
	TransformComponent transform{};
	transform.position_ = VGet(320.0f, 240.0f, 0.0f); 

	// エンティティにデータを紐づけてデータベースに登録
	ecsRegistry_.AddComponent<TransformComponent>(testPlayerEntity_, transform);
}

void SceneGame::Update(void)
{
	// テスト：OKボタン（Enterキーなど）を押したら右に少し移動させる
	if (KeyConfInputManager::GetInstance().isTrigerDown(L"OK"))
	{
		VECTOR moveVector = VGet(10.0f, 0.0f, 0.0f);
		transformSystem_.TranslatePosition(ecsRegistry_, testPlayerEntity_, moveVector);
	}

	// 毎フレーム必ずシステムを呼び出して、行列などの計算を最新にする
	transformSystem_.UpdateTransform(ecsRegistry_, testPlayerEntity_);
}

void SceneGame::Draw(void)
{
#ifdef _DEBUG
	const unsigned int WHITE_COLOR = GetColor(255, 255, 255);

	DrawString(100, 100, L"シーン : ゲーム本編 (ECSテスト中)", WHITE_COLOR, false);
	DrawString(100, 130, L"OKボタンを押すと座標が右に移動します", WHITE_COLOR, false);

	// エンティティがTransformを持っているか確認して現在地を描画
	if (ecsRegistry_.HasComponent<TransformComponent>(testPlayerEntity_))
	{
		TransformComponent& transform = ecsRegistry_.GetComponent<TransformComponent>(testPlayerEntity_);

		// 座標を画面に表示
		DrawFormatString(100, 160, WHITE_COLOR, L"Player Position: X=%.1f, Y=%.1f",
			transform.position_.x, transform.position_.y);
	}
#endif 
}

void SceneGame::Release(void)
{
}

void SceneGame::UpdateGui(void)
{
}