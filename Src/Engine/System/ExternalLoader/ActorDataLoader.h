#pragma once

#include <string>
#include <DxLib.h>

#pragma pack(push, 1)

/// @brief プレイヤーの初期パラメータを保持するバイナリ構造体
struct PlayerBinaryData
{
	// ファイル識別子
	char signature[4] = { 'P', 'L', 'R', '1' }; 

	// 移動・トランスフォーム関連
	VECTOR scale = { 50.0f, 50.0f, 50.0f };     // モデルのスケール
	float moveSpeed = 100.0f;                   // 移動速度
	float jumpPower = 150.0f;                   // ジャンプ力
	float maxSpeed = -1.0f;                     // 最大速度制限

	// カプセルコライダー関連
	VECTOR capsuleStartPos = { 0.0f, 30.0f, 0.0f }; // カプセル下端のローカル座標
	VECTOR capsuleEndPos = { 0.0f, 120.0f, 0.0f };  // カプセル上端のローカル座標
	float capsuleRadius = 30.0f;                    // カプセルの半径
};

/// @brief カメラの初期パラメータを保持するバイナリ構造体
struct CameraBinaryData
{
	// ファイル識別子
	char signature[4] = { 'C', 'A', 'M', '1' };      

	// パラメータ関連
	VECTOR targetOffset = { 0.0f, 50.0f, 0.0f };      // 注視点のズレ
	float distance = 300.0f;                          // ターゲットからの距離
	float angleHorizontal = 0.0f;                     // 初期水平角度
	float angleVertical = 15.0f * (DX_PI_F / 180.0f); // 初期垂直角度
	float rotationSpeed = 0.05f;                      // 回転速度
	float freeMoveSpeed = 2.0f;                       // フリーカメラ移動速度
	float fov = 60.0f * (DX_PI_F / 180.0f);           // 視野角
	float nearZ = 1.0f;                               // ニアクリップ
	float farZ = 20000.0f;                            // ファークリップ
};

struct EnemyBinaryData
{
	// ファイル識別子
	char signature[4] = { 'E', 'N', 'M', '1' };	

	// モデルのスケール
	VECTOR scale = { 30.0f, 30.0f, 30.0f };

	// カプセルコライダー関連
	VECTOR capsuleStartPos = { 0.0f, 10.0f, 0.0f };	// カプセル下端のローカル座標
	VECTOR capsuleEndPos = { 0.0f, 30.0f, 0.0f };	// カプセル上端のローカル座標
	float capsuleRadius = 30.0f;                    // カプセルの半径
	
	// レイコライダー関連
	VECTOR rayStartPos = { 0.0f, 50.0f, 0.0f };		// レイ始点のローカル座標
	VECTOR rayEndPos = { 0.0f, -5.0f, 0.0f };		// レイ終点のローカル座標
};

#pragma pack(pop)

/// @brief プレイヤーやカメラのパラメータをCSV変換・バイナリ読み書きするクラス
class ActorDataLoader
{
public:

	/// @brief プレイヤーデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadPlayerData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, PlayerBinaryData& outData);

	/// @brief プレイヤーデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するプレイヤーデータ
	/// @return 保存に成功したらtrue
	static bool SavePlayerData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const PlayerBinaryData& inData);

	/// @brief カメラデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadCameraData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, CameraBinaryData& outData);

	/// @brief カメラデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するカメラデータ
	/// @return 保存に成功したらtrue
	static bool SaveCameraData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const CameraBinaryData& inData);

	/// @brief エネミーデータを読み込む
	/// @param csvFilePath 開発用CSVファイルのパス
	/// @param binaryFilePath 実行用バイナリファイルのパス
	/// @param outData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadEnemyData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, EnemyBinaryData& outData);

	/// @brief エネミーデータをバイナリとCSVの両方に上書き保存する
	/// @param csvFilePath 保存先CSVファイルのパス
	/// @param binaryFilePath 保存先バイナリファイルのパス
	/// @param inData 保存するカメラデータ
	/// @return 保存に成功したらtrue
	static bool SaveEnemyData(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath, const EnemyBinaryData& inData);

private:

	/// @brief プレイヤー用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 
	static bool ConvertPlayerCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);

	/// @brief カメラ用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 
	static bool ConvertCameraCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);
	
	/// @brief エネミー用CSVをバイナリへ変換して保存する
	/// @param csvFilePath CSVファイルのパス
	/// @param binaryFilePath バイナリファイルのパス
	/// @return 
	static bool ConvertEnemyCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);
};