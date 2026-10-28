#include "../../../Pch.h"
#include "StageDataLoader.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// CSVの列インデックス関連
	constexpr int COLUMN_TYPE = 0;             // データ種別（SPAWN / OBJECT）
	constexpr int COLUMN_MODEL_KEY = 1;        // モデルのリソースキー名
	constexpr int COLUMN_POS_X = 2;            // X座標
	constexpr int COLUMN_POS_Y = 3;            // Y座標
	constexpr int COLUMN_POS_Z = 4;            // Z座標
	constexpr int COLUMN_ROT_X = 5;            // X回転（度数法）
	constexpr int COLUMN_ROT_Y = 6;            // Y回転（度数法）
	constexpr int COLUMN_ROT_Z = 7;            // Z回転（度数法）
	constexpr int COLUMN_SCALE_X = 8;          // Xスケール
	constexpr int COLUMN_SCALE_Y = 9;          // Yスケール
	constexpr int COLUMN_SCALE_Z = 10;         // Zスケール
	constexpr int COLUMN_PARENT_INDEX = 11;    // 親のインデックス
	constexpr int COLUMN_COL_SHAPE = 12;       // コライダー形状名
	constexpr int COLUMN_COL_TRIGGER = 13;     // 押し出しフラグ
	constexpr int COLUMN_COL_P1_X = 14;        // パラメータ1 X（中心/始点）
	constexpr int COLUMN_COL_P1_Y = 15;        // パラメータ1 Y
	constexpr int COLUMN_COL_P1_Z = 16;        // パラメータ1 Z
	constexpr int COLUMN_COL_P2_X = 17;        // パラメータ2 X（サイズ/終点）
	constexpr int COLUMN_COL_P2_Y = 18;        // パラメータ2 Y
	constexpr int COLUMN_COL_P2_Z = 19;        // パラメータ2 Z
	constexpr int COLUMN_COL_RADIUS = 20;      // 半径
	constexpr int COLUMN_COL_SELF_TAG = 21;    // 自身のタグ
	constexpr int COLUMN_COL_TARGET_MASK = 22; // 衝突対象タグマスク
	constexpr int REQUIRED_COLUMN_COUNT = 11;  // 最低限必要な列の総数

	// ColliderBase::SHAPE の定義順（0:NONE, 1:LINE, 2:SPHERE, 3:CAPSULE, 4:MODEL, 5:BOX）に合わせて変換
	int ParseShapeStringW(const std::wstring& shapeString)
	{
		if (shapeString == L"NONE")
		{
			return 0;
		}
		if (shapeString == L"LINE")
		{
			return 1;
		}
		if (shapeString == L"SPHERE")
		{
			return 2;
		}
		if (shapeString == L"CAPSULE")
		{
			return 3;
		}
		if (shapeString == L"MODEL")
		{
			return 4;
		}
		if (shapeString == L"BOX")
		{
			return 5;
		}
		return 5;
	}

	std::wstring ConvertShapeToStringW(int shapeType)
	{
		switch (shapeType)
		{
		case 0:
			return L"NONE";
		case 1:
			return L"LINE";
		case 2:
			return L"SPHERE";
		case 3:
			return L"CAPSULE";
		case 4:
			return L"MODEL";
		case 5:
			return L"BOX";
		default:
			return L"NONE";
		}
	}
}

bool StageDataLoader::ConvertCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::wifstream csvFile(csvFilePath); // CSV入力ストリーム
	if (!csvFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{};                    // バイナリヘッダー
	std::vector<StageObjectBinaryData> objectList; // オブジェクト配列
	std::wstring lineString;                       // 1行分の文字列

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

		std::wstringstream lineStream(lineString); // 行解析用ストリーム
		std::wstring cellString;                   // 1セル分の文字列
		std::vector<std::wstring> cells;           // 分割後のセル配列

		while (std::getline(lineStream, cellString, L','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::wstring& dataType = cells[COLUMN_TYPE]; // データ種別文字列

		if (dataType == L"SPAWN")
		{
			header.playerSpawnPosition.x = std::stof(cells[COLUMN_POS_X]);
			header.playerSpawnPosition.y = std::stof(cells[COLUMN_POS_Y]);
			header.playerSpawnPosition.z = std::stof(cells[COLUMN_POS_Z]);
		}
		else if (dataType == L"OBJECT")
		{
			StageObjectBinaryData objectData{}; // 1オブジェクト分のデータ
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

			std::wstring keyString(objectData.resourceKey); // リソースキー文字列
			if (keyString.find(L"GROUND") != std::wstring::npos)
			{
				objectData.colliderShape = 4; // MODEL
			}
			else
			{
				objectData.colliderShape = 5; // BOX
			}

			if (cells.size() > COLUMN_COL_SHAPE && !cells[COLUMN_COL_SHAPE].empty())
			{
				objectData.colliderShape = ParseShapeStringW(cells[COLUMN_COL_SHAPE]);
			}
			if (cells.size() > COLUMN_COL_TRIGGER && !cells[COLUMN_COL_TRIGGER].empty())
			{
				objectData.isTrigger = std::stoi(cells[COLUMN_COL_TRIGGER]);
			}
			if (cells.size() > COLUMN_COL_P1_Z && !cells[COLUMN_COL_P1_X].empty())
			{
				objectData.colParam1.x = std::stof(cells[COLUMN_COL_P1_X]);
				objectData.colParam1.y = std::stof(cells[COLUMN_COL_P1_Y]);
				objectData.colParam1.z = std::stof(cells[COLUMN_COL_P1_Z]);
			}
			if (cells.size() > COLUMN_COL_P2_Z && !cells[COLUMN_COL_P2_X].empty())
			{
				objectData.colParam2.x = std::stof(cells[COLUMN_COL_P2_X]);
				objectData.colParam2.y = std::stof(cells[COLUMN_COL_P2_Y]);
				objectData.colParam2.z = std::stof(cells[COLUMN_COL_P2_Z]);
			}
			if (cells.size() > COLUMN_COL_RADIUS && !cells[COLUMN_COL_RADIUS].empty())
			{
				objectData.colRadius = std::stof(cells[COLUMN_COL_RADIUS]);
			}
			if (cells.size() > COLUMN_COL_SELF_TAG && !cells[COLUMN_COL_SELF_TAG].empty())
			{
				objectData.selfTag = std::stoi(cells[COLUMN_COL_SELF_TAG]);
			}
			if (cells.size() > COLUMN_COL_TARGET_MASK && !cells[COLUMN_COL_TARGET_MASK].empty())
			{
				objectData.targetTagMask =
					static_cast<unsigned int>(std::stoul(cells[COLUMN_COL_TARGET_MASK]));
			}

			objectList.push_back(objectData);
		}
	}

	csvFile.close();

	header.objectCount = static_cast<unsigned int>(objectList.size());

	std::filesystem::path binaryPath(binaryFilePath); // バイナリ出力パス
	if (binaryPath.has_parent_path())
	{
		std::filesystem::create_directories(binaryPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
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
	std::ifstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ入力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{}; // 読み込んだヘッダー情報
	binaryFile.read(reinterpret_cast<char*>(&header), sizeof(StageBinaryHeader));

	if (header.signature[0] != 'S' || header.signature[1] != 'T' ||
		header.signature[2] != 'G' || header.signature[3] != '3')
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
	// バイナリファイルの保存
	std::filesystem::path binaryPath(binaryFilePath); // バイナリ保存パス
	if (binaryPath.has_parent_path())
	{
		std::filesystem::create_directories(binaryPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	StageBinaryHeader header{}; // 保存用ヘッダー
	header.playerSpawnPosition = inStageData.playerSpawnPosition;
	header.objectCount = static_cast<unsigned int>(inStageData.objects.size());

	binaryFile.write(reinterpret_cast<const char*>(&header), sizeof(StageBinaryHeader));
	if (header.objectCount > 0)
	{
		binaryFile.write(reinterpret_cast<const char*>(inStageData.objects.data()),
			sizeof(StageObjectBinaryData) * header.objectCount);
	}
	binaryFile.close();

	// CSVファイルの保存（度単位に変換して書き出し）
	std::filesystem::path csvPath(csvFilePath); // CSV保存パス
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::wofstream csvFile(csvFilePath); // CSV出力ストリーム
	if (!csvFile.is_open())
	{
		return false;
	}

	csvFile << L"Type,ModelId,PosX,PosY,PosZ,RotX,RotY,RotZ,"
		<< L"ScaleX,ScaleY,ScaleZ,ParentIndex,ColShape,IsTrigger,"
		<< L"ColP1X,ColP1Y,ColP1Z,ColP2X,ColP2Y,ColP2Z,ColRadius,"
		<< L"SelfTag,TargetMask\n";

	csvFile << L"SPAWN,NONE,"
		<< inStageData.playerSpawnPosition.x << L","
		<< inStageData.playerSpawnPosition.y << L","
		<< inStageData.playerSpawnPosition.z
		<< L",0,0,0,1,1,1,-1,NONE,0,0,0,0,0,0,0,0,0,0\n";

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
			<< ConvertShapeToStringW(objectData.colliderShape) << L","
			<< objectData.isTrigger << L","
			<< objectData.colParam1.x << L","
			<< objectData.colParam1.y << L","
			<< objectData.colParam1.z << L","
			<< objectData.colParam2.x << L","
			<< objectData.colParam2.y << L","
			<< objectData.colParam2.z << L","
			<< objectData.colRadius << L","
			<< objectData.selfTag << L","
			<< objectData.targetTagMask << L"\n";
	}

	csvFile.close();
	return true;
}