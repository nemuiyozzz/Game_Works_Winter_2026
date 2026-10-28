#include "../../../Pch.h"
#include "ActorDataLoader.h"
#include "../../Utility/UtilityMath.h"

namespace
{
	// CSVの列インデックス関連
	constexpr int COLUMN_PARAMETER_NAME = 0;  // パラメータ名
	constexpr int COLUMN_VALUE_X = 1;         // 値1（またはX成分）
	constexpr int COLUMN_VALUE_Y = 2;         // 値2（Y成分）
	constexpr int COLUMN_VALUE_Z = 3;         // 値3（Z成分）
	constexpr int REQUIRED_COLUMN_COUNT = 2;  // 最低限必要な列数
	constexpr int VECTOR_COLUMN_COUNT = 4;    // 3次元ベクトルに必要な列数

	int ConvertStringToShapeType(const std::string& shapeString)
	{
		if (shapeString == "NONE") { return 0; }
		if (shapeString == "LINE") { return 1; }
		if (shapeString == "SPHERE") { return 2; }
		if (shapeString == "CAPSULE") { return 3; }
		if (shapeString == "MODEL") { return 4; }
		if (shapeString == "BOX") { return 5; }
		return 0;
	}

	std::string ConvertShapeTypeToString(int shapeType)
	{
		switch (shapeType)
		{
		case 0:  return "NONE";
		case 1:  return "LINE";
		case 2:  return "SPHERE";
		case 3:  return "CAPSULE";
		case 4:  return "MODEL";
		case 5:  return "BOX";
		default: return "NONE";
		}
	}

	void WriteCollidersToCsvStream(
		std::ofstream& csvFile,
		int colliderCount,
		const ActorDataLoader::ColliderSlotBinaryData* colliders)
	{
		csvFile << "COLLIDER_COUNT," << colliderCount << ",0.0,0.0\n";

		for (int index = 0;
			index < colliderCount && index < ActorDataLoader::MAX_ACTOR_COLLIDERS;
			++index)
		{
			std::string prefix = "COLLIDER_" + std::to_string(index) + "_"; // 識別子接頭辞

			csvFile << prefix << "SHAPE,"
				<< ConvertShapeTypeToString(colliders[index].shapeType) << ",0.0,0.0\n";
			csvFile << prefix << "TRIGGER,"
				<< colliders[index].isTrigger << ",0.0,0.0\n";
			csvFile << prefix << "PARAM1,"
				<< colliders[index].vectorParameter1.x << ","
				<< colliders[index].vectorParameter1.y << ","
				<< colliders[index].vectorParameter1.z << "\n";
			csvFile << prefix << "PARAM2,"
				<< colliders[index].vectorParameter2.x << ","
				<< colliders[index].vectorParameter2.y << ","
				<< colliders[index].vectorParameter2.z << "\n";
			csvFile << prefix << "RADIUS,"
				<< colliders[index].radius << ",0.0,0.0\n";
		}
	}

	bool ParseColliderRowFromCsv(
		const std::string& parameterName,
		const std::vector<std::string>& cells,
		int& outColliderCount,
		ActorDataLoader::ColliderSlotBinaryData* outColliders)
	{
		if (parameterName == "COLLIDER_COUNT")
		{
			outColliderCount = std::stoi(cells[COLUMN_VALUE_X]);
			if (outColliderCount > ActorDataLoader::MAX_ACTOR_COLLIDERS)
			{
				outColliderCount = ActorDataLoader::MAX_ACTOR_COLLIDERS;
			}
			if (outColliderCount < 0)
			{
				outColliderCount = 0;
			}
			return true;
		}

		// 新形式（COLLIDER_0_SHAPE 等）の解析
		for (int index = 0; index < ActorDataLoader::MAX_ACTOR_COLLIDERS; ++index)
		{
			std::string prefix = "COLLIDER_" + std::to_string(index) + "_"; // 識別子接頭辞

			if (parameterName == prefix + "SHAPE")
			{
				outColliders[index].shapeType =
					ConvertStringToShapeType(cells[COLUMN_VALUE_X]);
				return true;
			}
			if (parameterName == prefix + "TRIGGER")
			{
				outColliders[index].isTrigger = std::stoi(cells[COLUMN_VALUE_X]);
				return true;
			}
			if (parameterName == prefix + "PARAM1" && cells.size() >= VECTOR_COLUMN_COUNT)
			{
				outColliders[index].vectorParameter1.x = std::stof(cells[COLUMN_VALUE_X]);
				outColliders[index].vectorParameter1.y = std::stof(cells[COLUMN_VALUE_Y]);
				outColliders[index].vectorParameter1.z = std::stof(cells[COLUMN_VALUE_Z]);
				return true;
			}
			if (parameterName == prefix + "PARAM2" && cells.size() >= VECTOR_COLUMN_COUNT)
			{
				outColliders[index].vectorParameter2.x = std::stof(cells[COLUMN_VALUE_X]);
				outColliders[index].vectorParameter2.y = std::stof(cells[COLUMN_VALUE_Y]);
				outColliders[index].vectorParameter2.z = std::stof(cells[COLUMN_VALUE_Z]);
				return true;
			}
			if (parameterName == prefix + "RADIUS")
			{
				outColliders[index].radius = std::stof(cells[COLUMN_VALUE_X]);
				return true;
			}
		}

		// 旧形式（CAPSULE_START / RAY_START 等）との互換読み込み
		if (parameterName == "CAPSULE_START" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			outColliders[0].shapeType = 3;
			outColliders[0].vectorParameter1.x = std::stof(cells[COLUMN_VALUE_X]);
			outColliders[0].vectorParameter1.y = std::stof(cells[COLUMN_VALUE_Y]);
			outColliders[0].vectorParameter1.z = std::stof(cells[COLUMN_VALUE_Z]);
			return true;
		}
		if (parameterName == "CAPSULE_END" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			outColliders[0].shapeType = 1;
			outColliders[0].vectorParameter2.x = std::stof(cells[COLUMN_VALUE_X]);
			outColliders[0].vectorParameter2.y = std::stof(cells[COLUMN_VALUE_Y]);
			outColliders[0].vectorParameter2.z = std::stof(cells[COLUMN_VALUE_Z]);
			return true;
		}
		if (parameterName == "CAPSULE_RADIUS")
		{
			outColliders[0].shapeType = 1;
			outColliders[0].radius = std::stof(cells[COLUMN_VALUE_X]);
			return true;
		}
		if (parameterName == "RAY_START" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			outColliders[1].shapeType = 1;
			outColliders[1].vectorParameter1.x = std::stof(cells[COLUMN_VALUE_X]);
			outColliders[1].vectorParameter1.y = std::stof(cells[COLUMN_VALUE_Y]);
			outColliders[1].vectorParameter1.z = std::stof(cells[COLUMN_VALUE_Z]);
			return true;
		}
		if (parameterName == "RAY_END" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			outColliders[1].shapeType = 3;
			outColliders[1].vectorParameter2.x = std::stof(cells[COLUMN_VALUE_X]);
			outColliders[1].vectorParameter2.y = std::stof(cells[COLUMN_VALUE_Y]);
			outColliders[1].vectorParameter2.z = std::stof(cells[COLUMN_VALUE_Z]);
			return true;
		}

		return false;
	}
}

bool ActorDataLoader::LoadPlayerData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, PlayerBinaryData& outData)
{
#ifdef _DEBUG
	if (std::filesystem::exists(csvFilePath))
	{
		ConvertPlayerCsvToBinary(csvFilePath, binaryFilePath);
	}
#endif

	std::ifstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ入力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	PlayerBinaryData loadedData{}; // 読み込んだプレイヤーデータ
	binaryFile.read(reinterpret_cast<char*>(&loadedData), sizeof(PlayerBinaryData));
	binaryFile.close();

	if (loadedData.signature[0] != 'P' || loadedData.signature[1] != 'L' ||
		loadedData.signature[2] != 'R' || loadedData.signature[3] != '2')
	{
		return false;
	}

	outData = loadedData;
	return true;
}

bool ActorDataLoader::SavePlayerData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const PlayerBinaryData& inData)
{
	std::filesystem::path binaryPath(binaryFilePath); // バイナリファイルパス
	if (binaryPath.has_parent_path())
	{
		std::filesystem::create_directories(binaryPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&inData), sizeof(PlayerBinaryData));
	binaryFile.close();

	std::filesystem::path csvPath(csvFilePath); // CSVファイルパス
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::ofstream csvFile(csvFilePath); // CSV出力ストリーム
	if (csvFile.is_open())
	{
		csvFile << "Parameter,ValueX,ValueY,ValueZ\n";
		csvFile << "SCALE,"
			<< inData.scale.x << ","
			<< inData.scale.y << ","
			<< inData.scale.z << "\n";
		csvFile << "MOVE_SPEED," << inData.moveSpeed << ",0.0,0.0\n";
		csvFile << "JUMP_POWER," << inData.jumpPower << ",0.0,0.0\n";
		csvFile << "MAX_SPEED," << inData.maxSpeed << ",0.0,0.0\n";
		csvFile << "MAX_HP," << inData.maxHp << ",0.0,0.0\n";
		csvFile << "ATTACK," << inData.attackPower << ",0.0,0.0\n";
		csvFile << "DEFENSE," << inData.defensePower << ",0.0,0.0\n";
		csvFile << "NEXT_EXP," << inData.nextLevelExperience << ",0.0,0.0\n";
		csvFile << "GROWTH_HP," << inData.growthHp << ",0.0,0.0\n";
		csvFile << "GROWTH_ATTACK," << inData.growthAttack << ",0.0,0.0\n";
		csvFile << "GROWTH_DEFENSE," << inData.growthDefense << ",0.0,0.0\n";

		WriteCollidersToCsvStream(csvFile, inData.colliderCount, inData.colliders);
		csvFile.close();
	}

	return true;
}

bool ActorDataLoader::LoadCameraData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, CameraBinaryData& outData)
{
#ifdef _DEBUG
	if (std::filesystem::exists(csvFilePath))
	{
		ConvertCameraCsvToBinary(csvFilePath, binaryFilePath);
	}
#endif

	std::ifstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ入力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	CameraBinaryData loadedData{}; // 読み込んだカメラデータ
	binaryFile.read(reinterpret_cast<char*>(&loadedData), sizeof(CameraBinaryData));
	binaryFile.close();

	if (loadedData.signature[0] != 'C' || loadedData.signature[1] != 'A' ||
		loadedData.signature[2] != 'M' || loadedData.signature[3] != '2')
	{
		return false;
	}

	outData = loadedData;
	return true;
}

bool ActorDataLoader::SaveCameraData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const CameraBinaryData& inData)
{
	std::filesystem::path binaryPath(binaryFilePath); // バイナリファイルパス
	if (binaryPath.has_parent_path())
	{
		std::filesystem::create_directories(binaryPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&inData), sizeof(CameraBinaryData));
	binaryFile.close();

	std::filesystem::path csvPath(csvFilePath); // CSVファイルパス
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::ofstream csvFile(csvFilePath); // CSV出力ストリーム
	if (csvFile.is_open())
	{
		csvFile << "Parameter,ValueX,ValueY,ValueZ\n";
		csvFile << "TARGET_OFFSET,"
			<< inData.targetOffset.x << ","
			<< inData.targetOffset.y << ","
			<< inData.targetOffset.z << "\n";
		csvFile << "DISTANCE," << inData.distance << ",0.0,0.0\n";
		csvFile << "ANGLE_H_DEG,"
			<< (UtilityMath::Rad2DegF(inData.angleHorizontal)) << ",0.0,0.0\n";
		csvFile << "ANGLE_V_DEG,"
			<< (UtilityMath::Rad2DegF(inData.angleVertical)) << ",0.0,0.0\n";
		csvFile << "ROTATION_SPEED," << inData.rotationSpeed << ",0.0,0.0\n";
		csvFile << "FREE_MOVE_SPEED," << inData.freeMoveSpeed << ",0.0,0.0\n";
		csvFile << "FOV_DEG," << (UtilityMath::Rad2DegF(inData.fov)) << ",0.0,0.0\n";
		csvFile << "NEAR_Z," << inData.nearZ << ",0.0,0.0\n";
		csvFile << "FAR_Z," << inData.farZ << ",0.0,0.0\n";

		WriteCollidersToCsvStream(csvFile, inData.colliderCount, inData.colliders);
		csvFile.close();
	}

	return true;
}

bool ActorDataLoader::LoadEnemyData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, EnemyBinaryData& outData)
{
#ifdef _DEBUG
	if (std::filesystem::exists(csvFilePath))
	{
		ConvertEnemyCsvToBinary(csvFilePath, binaryFilePath);
	}
#endif

	std::ifstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ入力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	EnemyBinaryData loadedData{}; // 読み込んだエネミーデータ
	binaryFile.read(reinterpret_cast<char*>(&loadedData), sizeof(EnemyBinaryData));
	binaryFile.close();

	if (loadedData.signature[0] != 'E' || loadedData.signature[1] != 'N' ||
		loadedData.signature[2] != 'M' || loadedData.signature[3] != '2')
	{
		return false;
	}

	outData = loadedData;
	return true;
}

bool ActorDataLoader::SaveEnemyData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const EnemyBinaryData& inData)
{
	std::filesystem::path binaryPath(binaryFilePath); // バイナリファイルパス
	if (binaryPath.has_parent_path())
	{
		std::filesystem::create_directories(binaryPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&inData), sizeof(EnemyBinaryData));
	binaryFile.close();

	std::filesystem::path csvPath(csvFilePath); // CSVファイルパス
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::ofstream csvFile(csvFilePath); // CSV出力ストリーム
	if (csvFile.is_open())
	{
		csvFile << "Parameter,ValueX,ValueY,ValueZ\n";
		csvFile << "SCALE,"
			<< inData.scale.x << ","
			<< inData.scale.y << ","
			<< inData.scale.z << "\n";
		csvFile << "MAX_HP," << inData.maxHp << ",0.0,0.0\n";
		csvFile << "ATTACK," << inData.attackPower << ",0.0,0.0\n";
		csvFile << "DEFENSE," << inData.defensePower << ",0.0,0.0\n";
		csvFile << "REWARD_EXP," << inData.rewardExperience << ",0.0,0.0\n";

		WriteCollidersToCsvStream(csvFile, inData.colliderCount, inData.colliders);
		csvFile.close();
	}

	return true;
}

bool ActorDataLoader::ConvertPlayerCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath); // CSV入力ストリーム
	if (!csvFile.is_open())
	{
		return false;
	}

	PlayerBinaryData playerData{}; // 変換用プレイヤーデータ
	std::string lineString;        // 1行分の文字列

	std::getline(csvFile, lineString);

	while (std::getline(csvFile, lineString))
	{
		if (lineString.empty())
		{
			continue;
		}

		std::stringstream lineStream(lineString); // 行解析用ストリーム
		std::string cellString;                   // 1セル分の文字列
		std::vector<std::string> cells;           // 分割後のセル配列

		while (std::getline(lineStream, cellString, ','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::string& parameterName = cells[COLUMN_PARAMETER_NAME]; // パラメータ名

		if (ParseColliderRowFromCsv(
			parameterName, cells, playerData.colliderCount, playerData.colliders))
		{
			continue;
		}

		if (parameterName == "SCALE" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			playerData.scale.x = std::stof(cells[COLUMN_VALUE_X]);
			playerData.scale.y = std::stof(cells[COLUMN_VALUE_Y]);
			playerData.scale.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (parameterName == "MOVE_SPEED")
		{
			playerData.moveSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "JUMP_POWER")
		{
			playerData.jumpPower = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "MAX_SPEED")
		{
			playerData.maxSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "MAX_HP")
		{
			playerData.maxHp = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "ATTACK")
		{
			playerData.attackPower = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "DEFENSE")
		{
			playerData.defensePower = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "NEXT_EXP")
		{
			playerData.nextLevelExperience = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "GROWTH_HP")
		{
			playerData.growthHp = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "GROWTH_ATTACK")
		{
			playerData.growthAttack = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "GROWTH_DEFENSE")
		{
			playerData.growthDefense = std::stoi(cells[COLUMN_VALUE_X]);
		}
	}

	csvFile.close();

	std::filesystem::path outputPath(binaryFilePath); // 出力先ファイルパス
	if (outputPath.has_parent_path())
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); // バイナリ出力ストリーム
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&playerData), sizeof(PlayerBinaryData));
	binaryFile.close();

	return true;
}

bool ActorDataLoader::ConvertCameraCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath); // CSV入力ストリーム
	if (!csvFile.is_open())
	{
		return false;
	}

	CameraBinaryData cameraData{}; // 変換用カメラデータ
	std::string lineString;        // 1行分の文字列

	std::getline(csvFile, lineString);

	while (std::getline(csvFile, lineString))
	{
		if (lineString.empty())
		{
			continue;
		}

		std::stringstream lineStream(lineString); // 行解析用ストリーム
		std::string cellString;                   // 1セル分の文字列
		std::vector<std::string> cells;           // 分割後のセル配列

		while (std::getline(lineStream, cellString, ','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::string& parameterName = cells[COLUMN_PARAMETER_NAME]; 

		if (ParseColliderRowFromCsv(
			parameterName, cells, cameraData.colliderCount, cameraData.colliders))
		{
			continue;
		}

		if (parameterName == "TARGET_OFFSET" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			cameraData.targetOffset.x = std::stof(cells[COLUMN_VALUE_X]);
			cameraData.targetOffset.y = std::stof(cells[COLUMN_VALUE_Y]);
			cameraData.targetOffset.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (parameterName == "DISTANCE")
		{
			cameraData.distance = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "ANGLE_H_DEG")
		{
			cameraData.angleHorizontal =
				UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (parameterName == "ANGLE_V_DEG")
		{
			cameraData.angleVertical =
				UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (parameterName == "ROTATION_SPEED")
		{
			cameraData.rotationSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "FREE_MOVE_SPEED")
		{
			cameraData.freeMoveSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "FOV_DEG")
		{
			cameraData.fov = UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (parameterName == "NEAR_Z")
		{
			cameraData.nearZ = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "FAR_Z")
		{
			cameraData.farZ = std::stof(cells[COLUMN_VALUE_X]);
		}
	}

	csvFile.close();

	std::filesystem::path outputPath(binaryFilePath); 
	if (outputPath.has_parent_path())
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary); 
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&cameraData), sizeof(CameraBinaryData));
	binaryFile.close();

	return true;
}

bool ActorDataLoader::ConvertEnemyCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath); // CSV入力ストリーム

	if (!csvFile.is_open())
	{
		return false;
	}

	EnemyBinaryData enemyData{}; // 変換用エネミーデータ
	std::string lineString;      // 1行分の文字列

	std::getline(csvFile, lineString);

	while (std::getline(csvFile, lineString))
	{
		if (lineString.empty())
		{
			continue;
		}

		std::stringstream lineStream(lineString); // 行解析用ストリーム
		std::string cellString;                   // 1セル分の文字列
		std::vector<std::string> cells;           // 分割後のセル配列

		while (std::getline(lineStream, cellString, ','))
		{
			cells.push_back(cellString);
		}

		if (cells.size() < REQUIRED_COLUMN_COUNT)
		{
			continue;
		}

		const std::string& parameterName = cells[COLUMN_PARAMETER_NAME];

		if (ParseColliderRowFromCsv(
			parameterName, cells, enemyData.colliderCount, enemyData.colliders))
		{
			continue;
		}

		if (parameterName == "SCALE" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			enemyData.scale.x = std::stof(cells[COLUMN_VALUE_X]);
			enemyData.scale.y = std::stof(cells[COLUMN_VALUE_Y]);
			enemyData.scale.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (parameterName == "MAX_HP")
		{
			enemyData.maxHp = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "ATTACK")
		{
			enemyData.attackPower = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "DEFENSE")
		{
			enemyData.defensePower = std::stoi(cells[COLUMN_VALUE_X]);
		}
		else if (parameterName == "REWARD_EXP")
		{
			enemyData.rewardExperience = std::stoi(cells[COLUMN_VALUE_X]);
		}
	}

	csvFile.close();

	std::filesystem::path outputPath(binaryFilePath); 
	if (outputPath.has_parent_path())
	{
		std::filesystem::create_directories(outputPath.parent_path());
	}

	std::ofstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	binaryFile.write(reinterpret_cast<const char*>(&enemyData), sizeof(EnemyBinaryData));
	binaryFile.close();

	return true;
}