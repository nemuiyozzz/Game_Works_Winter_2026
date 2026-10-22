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

	std::ifstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	PlayerBinaryData loadedData{};
	binaryFile.read(reinterpret_cast<char*>(&loadedData), sizeof(PlayerBinaryData));
	binaryFile.close();

	if (loadedData.signature[0] != 'P' || loadedData.signature[1] != 'L' ||
		loadedData.signature[2] != 'R' || loadedData.signature[3] != '1')
	{
		return false;
	}

	outData = loadedData;
	return true;
}

bool ActorDataLoader::SavePlayerData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const PlayerBinaryData& inData)
{
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

	binaryFile.write(reinterpret_cast<const char*>(&inData), sizeof(PlayerBinaryData));
	binaryFile.close();

	std::filesystem::path csvPath(csvFilePath);
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::ofstream csvFile(csvFilePath);
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
		csvFile << "CAPSULE_START,"
			<< inData.capsuleStartPos.x << ","
			<< inData.capsuleStartPos.y << ","
			<< inData.capsuleStartPos.z << "\n";
		csvFile << "CAPSULE_END,"
			<< inData.capsuleEndPos.x << ","
			<< inData.capsuleEndPos.y << ","
			<< inData.capsuleEndPos.z << "\n";
		csvFile << "CAPSULE_RADIUS," << inData.capsuleRadius << ",0.0,0.0\n";
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

	std::ifstream binaryFile(binaryFilePath, std::ios::binary);
	if (!binaryFile.is_open())
	{
		return false;
	}

	CameraBinaryData loadedData{};
	binaryFile.read(reinterpret_cast<char*>(&loadedData), sizeof(CameraBinaryData));
	binaryFile.close();

	if (loadedData.signature[0] != 'C' || loadedData.signature[1] != 'A' ||
		loadedData.signature[2] != 'M' || loadedData.signature[3] != '1')
	{
		return false;
	}

	outData = loadedData;
	return true;
}

bool ActorDataLoader::SaveCameraData(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath, const CameraBinaryData& inData)
{
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

	binaryFile.write(reinterpret_cast<const char*>(&inData), sizeof(CameraBinaryData));
	binaryFile.close();

	std::filesystem::path csvPath(csvFilePath);
	if (csvPath.has_parent_path())
	{
		std::filesystem::create_directories(csvPath.parent_path());
	}

	std::ofstream csvFile(csvFilePath);
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
		csvFile.close();
	}

	return true;
}

bool ActorDataLoader::ConvertPlayerCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath);
	if (!csvFile.is_open())
	{
		return false;
	}

	PlayerBinaryData playerData{};
	std::string lineString;

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

		const std::string& paramName = cells[COLUMN_PARAMETER_NAME];

		if (paramName == "SCALE" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			playerData.scale.x = std::stof(cells[COLUMN_VALUE_X]);
			playerData.scale.y = std::stof(cells[COLUMN_VALUE_Y]);
			playerData.scale.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (paramName == "MOVE_SPEED")
		{
			playerData.moveSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "JUMP_POWER")
		{
			playerData.jumpPower = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "MAX_SPEED")
		{
			playerData.maxSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "CAPSULE_START" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			playerData.capsuleStartPos.x = std::stof(cells[COLUMN_VALUE_X]);
			playerData.capsuleStartPos.y = std::stof(cells[COLUMN_VALUE_Y]);
			playerData.capsuleStartPos.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (paramName == "CAPSULE_END" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			playerData.capsuleEndPos.x = std::stof(cells[COLUMN_VALUE_X]);
			playerData.capsuleEndPos.y = std::stof(cells[COLUMN_VALUE_Y]);
			playerData.capsuleEndPos.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (paramName == "CAPSULE_RADIUS")
		{
			playerData.capsuleRadius = std::stof(cells[COLUMN_VALUE_X]);
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

	binaryFile.write(reinterpret_cast<const char*>(&playerData), sizeof(PlayerBinaryData));
	binaryFile.close();

	return true;
}

bool ActorDataLoader::ConvertCameraCsvToBinary(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
	std::ifstream csvFile(csvFilePath);
	if (!csvFile.is_open())
	{
		return false;
	}

	CameraBinaryData cameraData{};
	std::string lineString;

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

		const std::string& paramName = cells[COLUMN_PARAMETER_NAME];

		if (paramName == "TARGET_OFFSET" && cells.size() >= VECTOR_COLUMN_COUNT)
		{
			cameraData.targetOffset.x = std::stof(cells[COLUMN_VALUE_X]);
			cameraData.targetOffset.y = std::stof(cells[COLUMN_VALUE_Y]);
			cameraData.targetOffset.z = std::stof(cells[COLUMN_VALUE_Z]);
		}
		else if (paramName == "DISTANCE")
		{
			cameraData.distance = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "ANGLE_H_DEG")
		{
			cameraData.angleHorizontal =
				UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (paramName == "ANGLE_V_DEG")
		{
			cameraData.angleVertical =
				UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (paramName == "ROTATION_SPEED")
		{
			cameraData.rotationSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "FREE_MOVE_SPEED")
		{
			cameraData.freeMoveSpeed = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "FOV_DEG")
		{
			cameraData.fov = UtilityMath::Deg2RadF(std::stof(cells[COLUMN_VALUE_X]));
		}
		else if (paramName == "NEAR_Z")
		{
			cameraData.nearZ = std::stof(cells[COLUMN_VALUE_X]);
		}
		else if (paramName == "FAR_Z")
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