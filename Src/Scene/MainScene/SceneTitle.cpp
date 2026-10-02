#include "../../Pch.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "../SceneManager.h"
#include "../../System/Input/KeyConfInputManager.h"

SceneTitle::SceneTitle(void)
{
}

SceneTitle::~SceneTitle(void)
{
}

void SceneTitle::Load(void)
{
	// 基底クラスのロードフラグを立てる
	SceneBase::Load();
}

void SceneTitle::EndLoad(void)
{
	// 基底クラスのロードフラグを下ろす
	SceneBase::EndLoad();
}

void SceneTitle::Initialize(void)
{
	SceneBase::Initialize();
}

void SceneTitle::Update(void)
{
	if (KeyConfInputManager::GetInstance().isTrigerDown(L"OK"))
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>());
	}
}

void SceneTitle::Draw(void)
{
#ifdef _DEBUG
	const unsigned int WHITE_COLOR = GetColor(255, 255, 255);

	DrawString(100, 100, L"シーン：タイトル", WHITE_COLOR, false);
#endif 
}

void SceneTitle::Release(void)
{
}

void SceneTitle::UpdateGui(void)
{
}
