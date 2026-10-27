#include "../../../Pch.h"
#include "StageDataLoader.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// CSVの列インデックス関連
	constexpr int COLUMN_TYPE = 0;            // データ種別（SPAWN / OBJECT）
	constexpr int COLUMN_MODEL_KEY = 1;       // モデルのリソースキー名
	constexpr int COLUMN_POS_X = 2;           // X座標
	constexpr int COLUMN_POS_Y = 3;           // Y座標
	constexpr int COLUMN_POS_Z = 4;           // Z座標
	constexpr int COLUMN_ROT_X = 5;           // X回転（度数法）
	constexpr int COLUMN_ROT_Y = 6;           // Y回転（度数法）
	constexpr int COLUMN_ROT_Z = 7;           // Z回転（度数法）
	constexpr int COLUMN_SCALE_X = 8;         // Xスケール
	constexpr int COLUMN_SCALE_Y = 9;         // Yスケール
	constexpr int COLUMN_SCALE_Z = 10;        // Zスケール
	constexpr int COLUMN_PARENT_INDEX = 11;   // 親のインデックス
	constexpr int COLUMN_BOX_CENTER_X = 12;   // ボックス中心X
	constexpr int COLUMN_BOX_CENTER_Y = 13;   // ボックス中心Y
	constexpr int COLUMN_BOX_CENTER_Z = 14;   // ボックス中心Z
	constexpr int COLUMN_BOX_SIZE_X = 15;     // ボックスサイズX
	constexpr int COLUMN_BOX_SIZE_Y = 16;     // ボックスサイズY
	constexpr int COLUMN_BOX_SIZE_Z = 17;     // ボックスサイズZ
	constexpr int REQUIRED_COLUMN_COUNT = 11; // 最低限必要な列の総数
}

bool StageDataLoader::ConvertCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::wifstream csvFile(csvFilePath);
	if (!csvFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{};
	std::vector<StageObjectBinaryData> objectList;

	std::wstring lineString;

	// 1行目はヘッダー行として読み飛ばす
	std::getline(csvFile, lineString);

	while (std::getline(csvFile, lineString))
	{
		if (lineString.empty())
		{
			continue;
		}

		if (lineString.back() == L'\r')
		{
			lineString.pop_back();
		}

		std::wstringstream lineStream(lineString);
		std::wstring cellString;
		std::vector<std::wstring> cells;

		while (std::getline(lineStream, cellString, L','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::wstring& dataType = cells[COLUMN_TYPE];

		if (dataType == L"SPAWN")
		{
			header.playerSpawnPosition.x = std::stof(cells[COLUMN_POS_X]);
			header.playerSpawnPosition.y = std::stof(cells[COLUMN_POS_Y]);
			header.playerSpawnPosition.z = std::stof(cells[COLUMN_POS_Z]);
		}
		else if (dataType == L"OBJECT")
		{
			StageObjectBinaryData objectData{};
			wcsncpy_s(objectData.resourceKey, cells[COLUMN_MODEL_KEY].c_str(),
				MAX_RESOURCE_KEY_LENGTH - 1);

			objectData.position.x = std::stof(cells[COLUMN_POS_X]);
			objectData.position.y = std::stof(cells[COLUMN_POS_Y]);
			objectData.position.z = std::stof(cells[COLUMN_POS_Z]);

			objectData.rotation.x = UtilityMath::Deg2RadF(std::stof(cells[COLUMN_ROT_X]));
			objectData.rotation.y = UtilityMath::Deg2RadF(std::stof(cells[COLUMN_ROT_Y]));
			objectData.rotation.z = UtilityMath::Deg2RadF(std::stof(cells[COLUMN_ROT_Z]));

			objectData.scale.x = std::stof(cells[COLUMN_SCALE_X]);
			objectData.scale.y = std::stof(cells[COLUMN_SCALE_Y]);
			objectData.scale.z = std::stof(cells[COLUMN_SCALE_Z]);

			if (cells.size() > COLUMN_PARENT_INDEX && !cells[COLUMN_PARENT_INDEX].empty())
			{
				objectData.parentIndex = std::stoi(cells[COLUMN_PARENT_INDEX]);
			}

			if (cells.size() > COLUMN_BOX_CENTER_Z && !cells[COLUMN_BOX_CENTER_X].empty())
			{
				objectData.boxCenter.x = std::stof(cells[COLUMN_BOX_CENTER_X]);
				objectData.boxCenter.y = std::stof(cells[COLUMN_BOX_CENTER_Y]);
				objectData.boxCenter.z = std::stof(cells[COLUMN_BOX_CENTER_Z]);
			}

			if (cells.size() > COLUMN_BOX_SIZE_Z && !cells[COLUMN_BOX_SIZE_X].empty())
			{
				objectData.boxSize.x = std::stof(cells[COLUMN_BOX_SIZE_X]);
				objectData.boxSize.y = std::stof(cells[COLUMN_BOX_SIZE_Y]);
				objectData.boxSize.z = std::stof(cells[COLUMN_BOX_SIZE_Z]);
			}

			objectList.push_back(objectData);
		}
	}

	csvFile.close();

	header.objectCount = static_cast<unsigned int>(objectList.size());

	std::filesystem::path binPath(binaryFilePath);
	if (binPath.has_parent_path())
	{
		std::filesystem::create_directories(binPath.parent_path());
	}

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
		outStageData.objects.resize(header.objectCount);
		binaryFile.read(reinterpret_cast<char*>(outStageData.objects.data()),
			sizeof(StageObjectBinaryData) * header.objectCount);
	}

	binaryFile.close();
	return true;
}

bool StageDataLoader::SaveStageData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const LoadedStageData& inStageData)
{
	// 1. バイナリファイルの保存
	std::filesystem::path binPath(binaryFilePath);
	if (binPath.has_parent_path())
	{
		std::filesystem::create_directories(binPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{};
	header.playerSpawnPosition = inStageData.playerSpawnPosition;
	header.objectCount = static_cast<unsigned int>(inStageData.objects.size());

	binaryFile.write(reinterpret_cast<const char*>(&header), sizeof(StageBinaryHeader));
	if (header.objectCount > 0)
	{
		binaryFile.write(reinterpret_cast<const char*>(inStageData.objects.data()),
			sizeof(StageObjectBinaryData) * header.objectCount);
	}
	binaryFile.close();

	// 2. CSVファイルの保存（度単位に変換して書き出し）
	std::filesystem::path csvPath(csvFilePath);
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::wofstream csvFile(csvFilePath);
	if (!csvFile.is_open())
	{
		return false;
	}

	csvFile << L"Type,ModelId,PosX,PosY,PosZ,RotX,RotY,RotZ,ScaleX,ScaleY,ScaleZ,ParentIndex,BoxCenterX,BoxCenterY,BoxCenterZ,BoxSizeX,BoxSizeY,BoxSizeZ\n";
	csvFile << L"SPAWN,NONE,"
		<< inStageData.playerSpawnPosition.x << L","
		<< inStageData.playerSpawnPosition.y << L","
		<< inStageData.playerSpawnPosition.z << L",0,0,0,1,1,1,-1,0,0,0,0,0,0\n";

	for (const StageObjectBinaryData& objectData : inStageData.objects)
	{
		csvFile << L"OBJECT," << objectData.resourceKey << L","
			<< objectData.position.x << L","
			<< objectData.position.y << L","
			<< objectData.position.z << L","
			<< UtilityMath::Rad2DegF(objectData.rotation.x) << L","
			<< UtilityMath::Rad2DegF(objectData.rotation.y) << L","
			<< UtilityMath::Rad2DegF(objectData.rotation.z) << L","
			<< objectData.scale.x << L","
			<< objectData.scale.y << L","
			<< objectData.scale.z << L","
			<< objectData.parentIndex << L","
			<< objectData.boxCenter.x << L","
			<< objectData.boxCenter.y << L","
			<< objectData.boxCenter.z << L","
			<< objectData.boxSize.x << L","
			<< objectData.boxSize.y << L","
			<< objectData.boxSize.z << L"\n";
	}

	csvFile.close();
	return true;
}