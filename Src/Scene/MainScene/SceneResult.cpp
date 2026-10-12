#include "../../Pch.h"
#include "SceneResult.h"
#include "../SceneManager.h"
#include "../../System/Input/KeyConfInputManager.h"
#include "SceneTitle.h"

SceneResult::SceneResult(void)
{
}

SceneResult::~SceneResult(void)
{
}

void SceneResult::Load(void)
{
	SceneBase::Load();
}

void SceneResult::EndLoad(void)
{
	SceneBase::EndLoad();
}

void SceneResult::Initialize(void)
{
	SceneBase::Initialize();
}

void SceneResult::Update(void)
{
	if (KeyConfInputManager::GetInstance().isTrigerDown(L"OK"))
	{
		SceneManager::GetInstance().ChangeScene(std::make_shared<SceneTitle>());
	}
}

void SceneResult::Draw(void)
{
#ifdef _DEBUG
	const unsigned int WHITE_COLOR = GetColor(255, 255, 255);

	DrawString(100, 100, L"シーン : リザルト", WHITE_COLOR, false);
#endif 
}

void SceneResult::Release(void)
{
}

void SceneResult::UpdateGui(void)
{
}
