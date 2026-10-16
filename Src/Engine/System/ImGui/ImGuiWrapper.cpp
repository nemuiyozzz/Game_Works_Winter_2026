#include "../../../Pch.h"
#include "ImGuiWrapper.h"
#include "../../System/Input/KeyConfInputManager.h"
#include "../../../../Libs/ImGui/imgui.h"
#include "../../../../Libs/ImGui/backends/imgui_impl_dx11.h"
#include "../../../../Libs/ImGui/backends/imgui_impl_win32.h"

ImGuiWrapper* ImGuiWrapper::instance_ = nullptr;

// imgui_impl_win32.cppからのメッセージハンドラのプロトタイプ宣言
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND _windowHandle, UINT _message, WPARAM _wParam, LPARAM _lParam);

void ImGuiWrapper::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ImGuiWrapper();
	}
}

ImGuiWrapper& ImGuiWrapper::GetInstance(void)
{
	return *instance_;
}

void ImGuiWrapper::Initialize(void)
{
	// ウインドウへのメッセージをフックするウインドウプロージャを登録する
	DxLib::SetHookWinProc(WndProc);

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& imguiIO = ImGui::GetIO();
	imguiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	imguiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();
	if (imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(DxLib::GetMainWindowHandle());
	ImGui_ImplDX11_Init(
		(ID3D11Device*)DxLib::GetUseDirect3D11Device(),
		(ID3D11DeviceContext*)DxLib::GetUseDirect3D11DeviceContext());
}

void ImGuiWrapper::Update(void)
{
	// マウス入力制御
	UpdateInputMouse();

	// ImGui更新準備
	UpdateNewFrame();
}

void ImGuiWrapper::Draw(void)
{
	// ImGui描画
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& imguiIO = ImGui::GetIO();
	if (imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// 他の描画処理から、DxLib描画に戻る時に必要な処理
	DxLib::RefreshDxLibDirect3DSetting();
}

void ImGuiWrapper::DestroyInstance(void)
{
	// ImGuiの解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete instance_;
	instance_ = nullptr;
}

LRESULT ImGuiWrapper::WndProc(
	HWND _windowHandle, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	// このウインドウプロージャの戻り値を使用しない
	DxLib::SetUseHookWinProcReturnValue(false);

	// ImGuiにウィンドウイベントを渡す
	if (ImGui_ImplWin32_WndProcHandler(_windowHandle, _message, _wParam, _lParam))
	{
		return true;
	}

	const UINT SYSTEM_COMMAND_MASK = 0xfff0;

	switch (_message)
	{
	case WM_SIZE:
	{
		if (_wParam == SIZE_MINIMIZED)
		{
			return 0;
		}
		return 0;
	}
	case WM_SYSCOMMAND:
	{
		if ((_wParam & SYSTEM_COMMAND_MASK) == SC_KEYMENU)
		{
			return 0;
		}
		break;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		return 0;
	}
	}
	return ::DefWindowProcW(_windowHandle, _message, _wParam, _lParam);
}

ImGuiWrapper::~ImGuiWrapper(void)
{
}

void ImGuiWrapper::UpdateInputMouse(void)
{
	ImGuiIO& imguiIO = ImGui::GetIO();

	if ((imguiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) == 0)
	{
		Vector2 mousePosition = KeyConfInputManager::GetInstance().GetMousePosition();
		imguiIO.AddMousePosEvent(
			static_cast<float>(mousePosition.x_),
			static_cast<float>(mousePosition.y_));
	}

	// クリック状態はDxLibの関数から直接取得して判定
	int mouseInput = GetMouseInput();
	imguiIO.AddMouseButtonEvent(ImGuiMouseButton_Left, (mouseInput & MOUSE_INPUT_LEFT) != 0);
	imguiIO.AddMouseButtonEvent(ImGuiMouseButton_Right, (mouseInput & MOUSE_INPUT_RIGHT) != 0);
}

void ImGuiWrapper::UpdateNewFrame(void)
{
	// ImGui操作前準備
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

ImGuiWrapper::ImGuiWrapper(void)
{
}