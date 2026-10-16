#include "../../../Pch.h"
#include "StageDataLoader.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// CSVの列インデックス関連
	constexpr int COLUMN_TYPE = 0;     // データ種別（SPAWN / OBJECT）
	constexpr int COLUMN_MODEL_ID = 1; // モデルのリソースID
	constexpr int COLUMN_POS_X = 2;    // X座標
	constexpr int COLUMN_POS_Y = 3;    // Y座標
	constexpr int COLUMN_POS_Z = 4;    // Z座標
	constexpr int COLUMN_ROT_X = 5;    // X回転（度数法）
	constexpr int COLUMN_ROT_Y = 6;    // Y回転（度数法）
	constexpr int COLUMN_ROT_Z = 7;    // Z回転（度数法）
	constexpr int COLUMN_SCALE_X = 8;  // Xスケール
	constexpr int COLUMN_SCALE_Y = 9;  // Yスケール
	constexpr int COLUMN_SCALE_Z = 10; // Zスケール
	constexpr int REQUIRED_COLUMN_COUNT = 11; // 必要な列の総数

	// 角度変換用定数
	constexpr float DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

bool StageDataLoader::ConvertCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath);
	if (!csvFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{};
	std::vector<StageObjectBinaryData> objectList;

	std::string lineString;

	// 1行目はヘッダー行として読み飛ばす
	std::getline(csvFile, lineString);

	while (std::getline(csvFile, lineString))
	{
		if (lineString.empty())
		{
			continue;
		}

		std::stringstream lineStream(lineString);
		std::string cellString;
		std::vector<std::string> cells;

		while (std::getline(lineStream, cellString, ','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::string& dataType = cells[COLUMN_TYPE];

		if (dataType == "SPAWN")
		{
			header.playerSpawnPosition.x = std::stof(cells[COLUMN_POS_X]);
			header.playerSpawnPosition.y = std::stof(cells[COLUMN_POS_Y]);
			header.playerSpawnPosition.z = std::stof(cells[COLUMN_POS_Z]);
		}
		else if (dataType == "OBJECT")
		{
			StageObjectBinaryData objectData{};
			objectData.resourceId = std::stoi(cells[COLUMN_MODEL_ID]);

			objectData.position.x = std::stof(cells[COLUMN_POS_X]);
			objectData.position.y = std::stof(cells[COLUMN_POS_Y]);
			objectData.position.z = std::stof(cells[COLUMN_POS_Z]);

			objectData.rotation.x = std::stof(cells[COLUMN_ROT_X]) * DEGREE_TO_RADIAN;
			objectData.rotation.y = std::stof(cells[COLUMN_ROT_Y]) * DEGREE_TO_RADIAN;
			objectData.rotation.z = std::stof(cells[COLUMN_ROT_Z]) * DEGREE_TO_RADIAN;

			objectData.scale.x = std::stof(cells[COLUMN_SCALE_X]);
			objectData.scale.y = std::stof(cells[COLUMN_SCALE_Y]);
			objectData.scale.z = std::stof(cells[COLUMN_SCALE_Z]);

			objectList.push_back(objectData);
		}
	}

	csvFile.close();

	header.objectCount = static_cast<unsigned int>(objectList.size());

	// バイナリファイルとして一気に書き出す
	std::ofstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&header), sizeof(StageBinaryHeader));

	if (header.objectCount > 0)
	{
		binaryFile.write(reinterpret_cast<const char*>(objectList.data()),
			sizeof(StageObjectBinaryData) * header.objectCount);
	}

	binaryFile.close();
	return true;
}

bool StageDataLoader::LoadFromBinary(const std::wstring& binaryFilePath,
	LoadedStageData& outStageData)
{
	std::ifstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{};
	binaryFile.read(reinterpret_cast<char*>(&header), sizeof(StageBinaryHeader));

	// シグネチャが一致するかチェック
	if (header.signature[0] != 'S' || header.signature[1] != 'T' ||
		header.signature[2] != 'G' || header.signature[3] != '1')
	{
		binaryFile.close();
		return false;
	}

	outStageData.playerSpawnPosition = header.playerSpawnPosition;
	outStageData.objects.clear();

	if (header.objectCount > 0)
	{
		// 配列のサイズを確保して、ファイルからメモリへ直接流し込む
		outStageData.objects.resize(header.objectCount);
		binaryFile.read(reinterpret_cast<char*>(outStageData.objects.data()),
			sizeof(StageObjectBinaryData) * header.objectCount);
	}

	binaryFile.close();
	return true;
}