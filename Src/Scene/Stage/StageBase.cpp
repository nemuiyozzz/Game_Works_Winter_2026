#include "../../Pch.h"
#include "StageBase.h"
#include "../../Ecs/Component/TransformComponent.h"
#include "../../Common/Quaternion.h"

StageBase::StageBase(EcsRegistry& registry)
	: ecsRegistry_(registry)
{
}

StageBase::~StageBase(void)
{
}

void StageBase::Initialize(void)
{
	for (const StageObjectBinaryData& objectData : stageData_.objects)
	{
		Entity objectEntity = CreateStageEntity();

		TransformComponent transform{};
		transform.position = objectData.position;
		transform.rotationEuler = objectData.rotation;
		transform.rotation_ = Quaternion::Euler(
			objectData.rotation.x, objectData.rotation.y, objectData.rotation.z);
		transform.scale = objectData.scale;

		ecsRegistry_.AddComponent<TransformComponent>(objectEntity, transform);
	}
}

void StageBase::Release(void)
{
	for (Entity entity : stageEntities_)
	{
		ecsRegistry_.DestroyEntity(entity);
	}

	stageEntities_.clear();
}

Entity StageBase::CreateStageEntity(void)
{
	Entity entity = ecsRegistry_.CreateEntity();
	stageEntities_.push_back(entity);

	return entity;
}

void StageBase::LoadStageDataFile(const std::wstring& csvFilePath,
	const std::wstring& binaryFilePath)
{
#ifdef _DEBUG
	// デバッグ時はCSVが編集されていれば即座にバイナリへ変換・上書きする
	StageDataLoader::ConvertCsvToBinary(csvFilePath, binaryFilePath);
#endif

	// バイナリファイルから高速読み込みを行う
	StageDataLoader::LoadFromBinary(binaryFilePath, stageData_);
}
