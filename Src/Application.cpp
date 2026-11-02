#include "Pch.h"
#include "Application.h"
#include "Engine/System/Input/KeyConfInputManager.h"
#include "Engine/System/Fps/FpsController.h"
#include "Engine/System/Resource/ResourceManager.h"
#include "Engine/System/Time/TimeManager.h"
#include "Engine/System/ImGui/ImGuiWrapper.h"
#include "Scene/SceneManager.h"

Application* Application::instance_ = nullptr;

const std::wstring Application::PATH_DATA = L"Data/";
const std::wstring Application::PATH_IMAGE = L"Data/Image/";
const std::wstring Application::PATH_MODEL = L"Data/Model/";
const std::wstring Application::PATH_ANIM = L"Data/Animation/";
const std::wstring Application::PATH_EFFECT = L"Data/Effect/";
const std::wstring Application::PATH_SOUND = L"Data/Sound/";
const std::wstring Application::PATH_CSV = L"Data/Csv/";
const std::wstring Application::PATH_BINARY = L"Data/Binary/";
const std::wstring Application::PATH_SHADER = L"Data/Shader/";
const std::wstring Application::PATH_KEY_CONFIG = L"Data/Config/keyConfig.dat";
const std::wstring Application::PATH_SENSITIVITY = L"Data/Config/sensitivity.dat";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::DestroyInstance(void)
{
	SceneManager::GetInstance().DestroyInstance();
	ResourceManager::GetInstance().DestroyInstance();
	KeyConfInputManager::GetInstance().DestroyInstance();
	TimeManager::GetInstance().DestroyInstance();
	ImGuiWrapper::GetInstance().DestroyInstance();

	SetUseASyncLoadFlag(false);

	Effkseer_End();

	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	delete instance_;
}

Application::Application(void)
	: isInitFail_(false)
	, isReleaseFail_(false)
	, isGameEnd_(false)
{
}

void Application::Initialize(void)
{
	SetWindowText(L"MOUNIG");

	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);

#ifdef _DEBUG
	ChangeWindowMode(true);
#else
	ChangeWindowMode(false);
#endif 
	
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	SetAlwaysRunFlag(true);
	SetMultiThreadFlag(true);
	SetDoubleStartValidFlag(true);

	fpsController_ = std::make_unique<FpsController>(FRAME_RATE);

	SetChangeScreenModeGraphicsSystemResetFlag(false);

	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	if (!InitEffekseer())
	{
		isInitFail_ = true;
		return;
	}

	DATEDATA date;

	GetDateTime(&date);

	SRand(date.Year + date.Mon + date.Hour + date.Min + date.Sec);

	SetUseDirectInputFlag(true);

	KeyConfInputManager::CreateInstance();
	ImGuiWrapper::CreateInstance();
	ImGuiWrapper::GetInstance().Initialize();
	ResourceManager::CreateInstance();
	ResourceManager::GetInstance().Initialize();
	SceneManager::CreateInstance();
	SceneManager::GetInstance().Initialize();
	TimeManager::CreateInstance();
	TimeManager::GetInstance().Initialize();
}

void Application::Run(void)
{
	while (ProcessMessage() == 0)
	{
		if (isGameEnd_)
		{
			break;
		}

		ClearDrawScreen();

		if (KeyConfInputManager::GetInstance().IsTriggerDown(L"CANCEL"))
		{
			isGameEnd_ = true;
		}

		TimeManager::GetInstance().Update();
		KeyConfInputManager::GetInstance().Update();
		ImGuiWrapper::GetInstance().Update();
		SceneManager::GetInstance().Update();

		SceneManager::GetInstance().Draw();

		UpdateEffekseer3D();
		DrawEffekseer3D();

		fpsController_->Draw();

		RenderVertex();
		ImGuiWrapper::GetInstance().Draw();

		ScreenFlip();

		fpsController_->Wait();
	}
}

void Application::GameEnd(void)
{
	isGameEnd_ = true;
}

bool Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}

	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	return true;
}