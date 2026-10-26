#pragma once

#include <string>

/// @brief ImGuiでの保存処理の種類
enum class DEBUG_SAVE_TYPE
{
	NONE,   // 保存ボタンなし（確認・調整のみ）
	PLAYER, // プレイヤーデータとして保存
	ENEMY,	// エネミーデータとして保存
	CAMERA, // カメラデータとして保存
	STAGE,  // ステージデータとして保存
};

/// @brief ImGuiデバッグウィンドウの表示対象であることを示すコンポーネント
struct DebugGuiComponent
{
	// ウィンドウ・保存設定関連
	std::string windowTitle = "Entity";               // ImGuiのウィンドウ名
	DEBUG_SAVE_TYPE saveType = DEBUG_SAVE_TYPE::NONE; // 保存データの種類
	std::wstring csvFilePath = L"";                   // 保存先CSVファイルパス
	std::wstring binaryFilePath = L"";                // 保存先バイナリファイルパス
	std::wstring stageResourceKey = L"";                 // ステージモデルのリソース名
};