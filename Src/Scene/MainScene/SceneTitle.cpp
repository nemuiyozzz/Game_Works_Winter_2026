#include "../../Pch.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "../SceneManager.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../../Application.h"
#include "../../Ecs/Component/ImageComponent.h"     
#include "../../Ecs/Component/TransformComponent.h" 
#include "../../Ecs/Component/Collision2DComponent.h"
#include "../../Ecs/System/ImageSystem.h"
#include "../../Ecs/System/TransformSystem.h"
#include "../../Ecs/System/Collision2DSystem.h"
#include "../../Engine/Physics/Collider2D/Collider2DCircle.h"
#include "../../Engine/Physics/Collider2D/Collider2DBox.h"

SceneTitle::SceneTitle(void)
{
}

SceneTitle::~SceneTitle(void)
{
}

void SceneTitle::Load(void)
{
	SceneBase::Load();

	ResourceManager::GetInstance().Load(L"IMAGE_TITLE"); 
	ResourceManager::GetInstance().Load(L"IMAGES_TITLE_FONT");
}

void SceneTitle::EndLoad(void)
{
	// 基底クラスのロードフラグを下ろす
	SceneBase::EndLoad();
}

void SceneTitle::Initialize(void)
{
	SceneBase::Initialize();

	ecsWorld_.Initialize();

	Entity logoEntity = ecsWorld_.GetRegistry().CreateEntity();

	int handle = ResourceManager::GetInstance().GetHandleId(L"IMAGE_TITLE");
	ImageComponent imageComp{};
	imageComp.imageHandle = handle;
	imageComp.isTransparent = true;
	ecsWorld_.GetRegistry().AddComponent<ImageComponent>(logoEntity, imageComp);

	TransformComponent transform{};
	if (handle != -1)
	{
		transform.scale = VGet(0.5f, 0.5f, 0.5f);
		transform.position.x = Application::SCREEN_HALF_X;
		transform.position.y = Application::SCREEN_HALF_Y - 50.0f;
	}
	ecsWorld_.GetRegistry().AddComponent<TransformComponent>(logoEntity, transform);

	Resource fontRes = ResourceManager::GetInstance().Load(L"IMAGES_TITLE_FONT");
	menuHandles_.resize(4, -1);
	fontRes.CopyHandle(menuHandles_.data());

	cursorEntity_ = ecsWorld_.GetRegistry().CreateEntity();
	Collision2DComponent cursorColComp{};
	auto cursorCol = std::make_shared<Collider2DCircle>(
		Vector2F(0.0f, 0.0f), 1.0f, Collider2DBase::TAG_2D::MOUSE_CURSOR);
	cursorCol->AddTargetTag(Collider2DBase::TAG_2D::BUTTON_NEWGAME);
	cursorCol->AddTargetTag(Collider2DBase::TAG_2D::BUTTON_QUITGAME);
	cursorColComp.colliders.push_back(cursorCol);
	ecsWorld_.GetRegistry().AddComponent<Collision2DComponent>(cursorEntity_, cursorColComp);
	ecsWorld_.GetRegistry().AddComponent<TransformComponent>(cursorEntity_, TransformComponent{});

	newGameEntity_ = ecsWorld_.GetRegistry().CreateEntity();

	ImageComponent newGameImg{};
	newGameImg.imageHandle = menuHandles_[2];
	newGameImg.isTransparent = true;
	ecsWorld_.GetRegistry().AddComponent<ImageComponent>(newGameEntity_, newGameImg);

	TransformComponent newGameTransform{};
	newGameTransform.scale = VGet(0.3f, 0.3f, 0.3f);
	newGameTransform.position.x = Application::SCREEN_HALF_X;
	newGameTransform.position.y = Application::SCREEN_HALF_Y + 200.0f;
	ecsWorld_.GetRegistry().AddComponent<TransformComponent>(newGameEntity_, newGameTransform);

	Collision2DComponent newGameColComp{};
	auto newGameCol = std::make_shared<Collider2DBox>(
		Vector2F(newGameTransform.position.x, newGameTransform.position.y),
		250.0f, 50.0f, Collider2DBase::TAG_2D::BUTTON_NEWGAME);
	newGameCol->AddTargetTag(Collider2DBase::TAG_2D::MOUSE_CURSOR);
	newGameColComp.colliders.push_back(newGameCol);
	ecsWorld_.GetRegistry().AddComponent<Collision2DComponent>(newGameEntity_, newGameColComp);

	quitGameEntity_ = ecsWorld_.GetRegistry().CreateEntity();

	ImageComponent quitImg{};
	quitImg.imageHandle = menuHandles_[1];
	quitImg.isTransparent = true;
	ecsWorld_.GetRegistry().AddComponent<ImageComponent>(quitGameEntity_, quitImg);

	TransformComponent quitTransform{};
	quitTransform.scale = VGet(0.3f, 0.3f, 0.3f);
	quitTransform.position.x = Application::SCREEN_HALF_X;
	quitTransform.position.y = Application::SCREEN_HALF_Y + 300.0f;
	ecsWorld_.GetRegistry().AddComponent<TransformComponent>(quitGameEntity_, quitTransform);

	Collision2DComponent quitColComp{};
	auto quitCol = std::make_shared<Collider2DBox>(
		Vector2F(quitTransform.position.x, quitTransform.position.y),
		250.0f, 50.0f, Collider2DBase::TAG_2D::BUTTON_QUITGAME);
	quitCol->AddTargetTag(Collider2DBase::TAG_2D::MOUSE_CURSOR);
	quitColComp.colliders.push_back(quitCol);
	ecsWorld_.GetRegistry().AddComponent<Collision2DComponent>(quitGameEntity_, quitColComp);

	selectedMenuIndex_ = 0;
	isMouseMode_ = true;
}

void SceneTitle::Update(void)
{
	auto& input = KeyConfInputManager::GetInstance();
	auto& registry = ecsWorld_.GetRegistry();

	const Vector2 mousePos = input.GetMousePosition();
	const Vector2F mousePosF(static_cast<float>(mousePos.x_), static_cast<float>(mousePos.y_));

	if (mousePosF.x_ != prevMousePos_.x_ || mousePosF.y_ != prevMousePos_.y_)
	{
		isMouseMode_ = true;
	}
	
	if (input.IsTriggerDown(L"UP") || input.IsTriggerDown(L"DOWN") ||
		input.IsTriggerDown(L"OK") || input.IsTriggerDown(L"CANCEL"))
	{
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) == 0)
		{
			isMouseMode_ = false;
		}
	}
	prevMousePos_ = mousePosF;

	TransformComponent& cursorTrans = registry.GetComponent<TransformComponent>(cursorEntity_);
	if (isMouseMode_)
	{
		cursorTrans.position = VGet(mousePosF.x_, mousePosF.y_, 0.0f);

		auto& colComp = registry.GetComponent<Collision2DComponent>(cursorEntity_);
		auto circleCol = std::static_pointer_cast<Collider2DCircle>(colComp.colliders[0]);
		circleCol->SetCenterPos(mousePosF);
	}
	else
	{
		cursorTrans.position = VGet(-1000.0f, -1000.0f, 0.0f); 
		auto& colComp = registry.GetComponent<Collision2DComponent>(cursorEntity_);
		auto circleCol = std::static_pointer_cast<Collider2DCircle>(colComp.colliders[0]);
		circleCol->SetCenterPos(Vector2F(-1000.0f, -1000.0f));
	}

	if (!isMouseMode_)
	{
		if (input.IsTriggerDown(L"UP") || input.IsTriggerDown(L"DOWN"))
		{
			selectedMenuIndex_ = (selectedMenuIndex_ == 0) ? 1 : 0;
		}
	}

	ecsWorld_.Update();

	bool isHoveringAnyButton = false;

	if (isMouseMode_)
	{
		auto& newGameCol = registry.GetComponent<Collision2DComponent>(newGameEntity_);
		auto& quitCol = registry.GetComponent<Collision2DComponent>(quitGameEntity_);

		if (!newGameCol.hitEntities.empty())
		{
			selectedMenuIndex_ = 0;
			isHoveringAnyButton = true;
		}
		else if (!quitCol.hitEntities.empty())
		{
			selectedMenuIndex_ = 1;
			isHoveringAnyButton = true;
		}
	}

	registry.GetComponent<ImageComponent>(newGameEntity_).imageHandle =
		(selectedMenuIndex_ == 0) ? menuHandles_[2] : menuHandles_[0];

	registry.GetComponent<ImageComponent>(quitGameEntity_).imageHandle =
		(selectedMenuIndex_ == 1) ? menuHandles_[3] : menuHandles_[1];

	if (input.IsTriggerDown(L"OK"))
	{
		bool canExecute = true;
		if (isMouseMode_ && !isHoveringAnyButton)
		{
			canExecute = false;
		}

		if (canExecute)
		{
			if (selectedMenuIndex_ == 0)
			{
				SceneManager::GetInstance().ChangeScene(std::make_shared<SceneGame>());
			}
			else if (selectedMenuIndex_ == 1)
			{
				Application::GetInstance().GameEnd();
			}
		}
	}
}

void SceneTitle::Draw(void)
{
	ecsWorld_.Draw();

#ifdef _DEBUG
	const unsigned int WHITE_COLOR = GetColor(255, 255, 255);
	DrawString(100, 100, L"シーン：タイトル", WHITE_COLOR, false);

	auto& trans = ecsWorld_.GetRegistry().GetComponent<TransformComponent>(newGameEntity_);
	DrawFormatString(100, 150, WHITE_COLOR, L"Handles: [%d, %d, %d, %d] / Y Pos: %.1f",
		menuHandles_[0], menuHandles_[1], menuHandles_[2], menuHandles_[3], trans.position.y);
#endif 
}

void SceneTitle::Release(void)
{
	ecsWorld_.Release();
}