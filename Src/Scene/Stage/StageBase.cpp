#include "../../Pch.h"
#include "StageBase.h"
#include "../../Ecs/Component/TransformComponent.h"
#include "../../Ecs/Component/ModelComponent.h"
#include "../../Ecs/Component/DebugGuiComponent.h"
#include "../../Ecs/Component/HierarchyComponent.h"
#include "../../Ecs/Component/CollisionComponent.h"
#include "../../Engine/Physics/Collider/ColliderModel.h"
#include "../../Engine/Physics/Collider/ColliderBox.h"
#include "../../Engine/System/Resource/ResourceManager.h"
#include "../../Common/Quaternion.h"

StageBase::StageBase(EcsRegistry& registry)
	: ecsRegistry_(registry)
	, csvFilePath_(L"")
	, binaryFilePath_(L"")
{
}

StageBase::~StageBase(void)
{
}

void StageBase::Initialize(void)
{
	std::vector<Entity> indexToEntityMap(stageData_.objects.size(), NULL_ENTITY);

	for (size_t index = 0; index < stageData_.objects.size(); ++index)
	{
		const StageObjectBinaryData& objectData = stageData_.objects[index];
		Entity objectEntity = CreateStageEntity();

		indexToEntityMap[index] = objectEntity;

		TransformComponent transform{};
		transform.position = objectData.position;
		transform.localPosition = objectData.position;
		transform.rotationEuler = objectData.rotation;
		transform.rotation_ = Quaternion::Euler(
			objectData.rotation.x, objectData.rotation.y, objectData.rotation.z);
		transform.scale = objectData.scale;
		ecsRegistry_.AddComponent<TransformComponent>(objectEntity, transform);

		// 3Dモデルの複製と付与
		std::wstring resourceKey(objectData.resourceKey);
		if (!resourceKey.empty() && resourceKey != L"NONE" && resourceKey != L"0")
		{
			Resource loadedRes = ResourceManager::GetInstance().Load(resourceKey);
			int modelHandle = -1;
			if (loadedRes.GetHandleId() != -1)
			{
				modelHandle = MV1DuplicateModel(loadedRes.GetHandleId());
			}
			if (modelHandle == -1)
			{
				modelHandle = loadedRes.GetHandleId();
			}
			if (modelHandle != -1)
			{
				ModelComponent modelComponent{};
				modelComponent.modelHandle = modelHandle;
				ecsRegistry_.AddComponent<ModelComponent>(objectEntity, modelComponent);

				CollisionComponent collisionComp{};

				auto stageTag = ColliderBase::TAG::STAGE;

				if (resourceKey.find(L"GROUND") != std::wstring::npos)
				{
					MV1SetPosition(modelHandle, objectData.position);
					MV1SetRotationXYZ(modelHandle, objectData.rotation);
					MV1SetScale(modelHandle, objectData.scale);

					auto modelCollider = std::make_shared<ColliderModel>(stageTag, nullptr);

					modelCollider->SetModelHandle(modelHandle);
					modelCollider->AddTargetTag(ColliderBase::TAG::PLAYER);
					modelCollider->AddTargetTag(ColliderBase::TAG::ENEMY); 

					collisionComp.colliders.push_back(modelCollider);
				}
				else
				{
					auto boxCollider = std::make_shared<ColliderBox>(
						stageTag,
						nullptr,
						objectData.boxCenter, 
						objectData.boxSize   
					);

					boxCollider->AddTargetTag(ColliderBase::TAG::PLAYER);
					boxCollider->AddTargetTag(ColliderBase::TAG::ENEMY); 
					collisionComp.colliders.push_back(boxCollider);
				}

				ecsRegistry_.AddComponent<CollisionComponent>(objectEntity, collisionComp);
			}
		}

#ifdef _DEBUG
		std::string narrowKeyName = "";
		for (wchar_t wideChar : resourceKey)
		{
			narrowKeyName.push_back(static_cast<char>(wideChar));
		}
		DebugGuiComponent stageGui{};
		stageGui.windowTitle = "StageObj[" + std::to_string(index) + "] " + narrowKeyName;
		stageGui.saveType = DEBUG_SAVE_TYPE::STAGE;
		stageGui.csvFilePath = csvFilePath_;
		stageGui.binaryFilePath = binaryFilePath_;
		stageGui.stageResourceKey = resourceKey;
		ecsRegistry_.AddComponent<DebugGuiComponent>(objectEntity, stageGui);
#endif // _DEBUG
	}

	for (size_t index = 0; index < stageData_.objects.size(); ++index)
	{
		const StageObjectBinaryData& objectData = stageData_.objects[index];
		Entity childEntity = indexToEntityMap[index];

		if (objectData.parentIndex >= 0 && objectData.parentIndex < static_cast<int>(indexToEntityMap.size()))
		{
			Entity parentEntity = indexToEntityMap[objectData.parentIndex];

			if (!ecsRegistry_.HasComponent<HierarchyComponent>(childEntity))
			{
				ecsRegistry_.AddComponent<HierarchyComponent>(childEntity, HierarchyComponent{});
			}
			ecsRegistry_.GetComponent<HierarchyComponent>(childEntity).parent = parentEntity;

			if (!ecsRegistry_.HasComponent<HierarchyComponent>(parentEntity))
			{
				ecsRegistry_.AddComponent<HierarchyComponent>(parentEntity, HierarchyComponent{});
			}
			ecsRegistry_.GetComponent<HierarchyComponent>(parentEntity).children.push_back(childEntity);
		}
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
	csvFilePath_ = csvFilePath;
	binaryFilePath_ = binaryFilePath;

#ifdef _DEBUG
	// デバッグ時はCSVが編集されていれば即座にバイナリへ変換・上書きする
	StageDataLoader::ConvertCsvToBinary(csvFilePath, binaryFilePath);
#endif

	// バイナリファイルから高速読み込みを行う
	StageDataLoader::LoadFromBinary(binaryFilePath, stageData_);

	// ステージで使用する全モデルリソースの非同期ロードをここでキックする
	for (const StageObjectBinaryData& objectData : stageData_.objects)
	{
		std::wstring resourceKey(objectData.resourceKey);
		if (!resourceKey.empty() && resourceKey != L"NONE" && resourceKey != L"0")
		{
			ResourceManager::GetInstance().Load(resourceKey);
		}
	}
}