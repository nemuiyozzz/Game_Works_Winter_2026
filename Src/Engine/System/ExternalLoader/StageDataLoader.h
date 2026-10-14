#pragma once

#include <string>
#include <vector>
#include <DxLib.h>

#pragma pack(push, 1)

/// @brief バイナリファイルの先頭に記録するヘッダー情報
struct StageBinaryHeader
{
	char signature[4] = { 'S', 'T', 'G', '1' };			// ファイル識別子
	VECTOR playerSpawnPosition = { 0.0f, 0.0f, 0.0f };	// プレイヤー初期位置
	unsigned int objectCount = 0;						// 配置オブジェクトの総数
};

/// @brief ステージ上に配置する1オブジェクト分のバイナリデータ
struct StageObjectBinaryData
{
	int resourceId = -1;				    // リソースID
	VECTOR position = { 0.0f, 0.0f, 0.0f }; // 座標
	VECTOR rotation = { 0.0f, 0.0f, 0.0f }; // 回転
	VECTOR scale = { 1.0f, 1.0f, 1.0f };    // スケール
};

#pragma pack(pop)

/// @brief 読み込んだステージデータを保持する構造体
struct LoadedStageData
{
	VECTOR playerSpawnPosition = { 0.0f, 0.0f, 0.0f }; // プレイヤー初期位置
	std::vector<StageObjectBinaryData> objects;        // 配置オブジェクト配列
};

/// @brief CSVからバイナリへの変換およびバイナリからのステージデータ読み込みを行うクラス
class StageDataLoader
{
public:

	/// @brief CSVファイルを読み込み、バイナリファイルへ変換して保存する
	/// @param csvFilePath 読み込むCSVファイルのパス
	/// @param binaryFilePath 出力するバイナリファイルのパス
	/// @return 変換に成功したらtrue
	static bool ConvertCsvToBinary(const std::wstring& csvFilePath,
		const std::wstring& binaryFilePath);

	/// @brief バイナリファイルからステージデータを高速に読み込む
	/// @param binaryFilePath 読み込むバイナリファイルのパス
	/// @param outStageData 読み込んだデータの格納先
	/// @return 読み込みに成功したらtrue
	static bool LoadFromBinary(const std::wstring& binaryFilePath,
		LoadedStageData& outStageData);
};

