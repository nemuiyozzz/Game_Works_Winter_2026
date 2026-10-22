#pragma once

#include <Windows.h>

/// @brief ImGuiのラッパークラス
class ImGuiWrapper
{
public:

	/// @brief インスタンスの生成
	static void CreateInstance(void);

	/// @brief インスタンスの取得
	/// @return ImGuiWrapperの参照
	static ImGuiWrapper& GetInstance(void);

	/// @brief 初期化
	void Initialize(void);

	/// @brief 更新
	void Update(void);

	/// @brief 描画
	void Draw(void);

	/// @brief 破棄
	void DestroyInstance(void);

private:

	// 静的インスタンス
	static ImGuiWrapper* instance_; 

	/// @brief ウインドウプロシージャ
	/// @param _windowHandle ウィンドウハンドル
	/// @param _message メッセージ
	/// @param _wParam 追加のメッセージ情報
	/// @param _lParam 追加のメッセージ情報
	/// @return メッセージ処理の結果
	static LRESULT CALLBACK WndProc(
		HWND _windowHandle, UINT _message, WPARAM _wParam, LPARAM _lParam);

	/// @brief デフォルトコンストラクタをprivateにして、外部から生成できない様にする
	ImGuiWrapper(void);

	/// @brief デストラクタ
	~ImGuiWrapper(void);

	// コピーと代入を禁止
	ImGuiWrapper(const ImGuiWrapper&) = delete;
	ImGuiWrapper& operator=(const ImGuiWrapper&) = delete;
	ImGuiWrapper(ImGuiWrapper&&) = delete;
	ImGuiWrapper& operator=(ImGuiWrapper&&) = delete;

	/// @brief マウス入力の更新
	void UpdateInputMouse(void);

	/// @brief ImGuiの新しいフレームの更新
	void UpdateNewFrame(void);
};