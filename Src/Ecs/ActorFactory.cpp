#include "../Pch.h"
#include "ActorFactory.h"
#include "EcsRegistry.h"
#include "../Application.h"
#include "../Engine/System/Resource/ResourceManager.h"
#include "../Engine/System/ExternalLoader/ActorDataLoader.h"
#include "../Engine/Physics/Collider/ColliderSphere.h"
#include "../Engine/Physics/Collider/ColliderCapsule.h"
#include "../Engine/Physics/Collider/ColliderBox.h"
#include "../Engine/Physics/Collider/ColliderLine.h"
#include "../Engine/Physics/Collider/ColliderModel.h"
#include "Component/TransformComponent.h"
#include "Component/VelocityComponent.h"
#include "Component/PlayerInputComponent.h"
#include "Component/ModelComponent.h"
#include "Component/CameraComponent.h"
#include "Component/CollisionComponent.h"
#include "Component/DebugGuiComponent.h"
#include "Component/EnemyComponent.h"
#include "Component/AnimationComponent.h"
#include "Component/HierarchyComponent.h"
#include "System/AnimationSystem.h"
#include "System/PlayerControlSystem.h"
#include "Component/StatusComponent.h"

namespace
{
	// ファイル名関連
	constexpr const wchar_t* PLAYER_CSV_NAME = L"PlayerParam.csv"; // プレイヤーCSV名
	constexpr const wchar_t* PLAYER_BIN_NAME = L"PlayerParam.bin"; // プレイヤーバイナリ名
	constexpr const wchar_t* CAMERA_CSV_NAME = L"CameraParam.csv"; // カメラCSV名
	constexpr const wchar_t* CAMERA_BIN_NAME = L"CameraParam.bin"; // カメラバイナリ名
	constexpr const wchar_t* ENEMY_CSV_NAME = L"EnemyParam.csv";   // エネミーCSV名
	constexpr const wchar_t* ENEMY_BIN_NAME = L"EnemyParam.bin";   // エネミーバイナリ名

	// リソース・レシピキー名関連
	constexpr const wchar_t* KEY_MODEL_PLAYER = L"MODEL_PLAYER"; // プレイヤーモデル
	constexpr const wchar_t* RECIPE_PLAYER = L"Player";          // プレイヤーレシピ名
	constexpr const wchar_t* KEY_MODEL_ENEMY = L"MODEL_SAMPLE";  // エネミーモデル
	constexpr const wchar_t* RECIPE_ENEMY = L"Enemy";            // エネミーレシピ名
	constexpr const wchar_t* KEY_MODEL_SWORD = L"MODEL_SWORD";   // 剣モデル
	constexpr const wchar_t* RECIPE_SWORD = L"Sword";            // 剣レシピ名

	/// @brief バイナンスロットデータから対応するコライダーインスタンスを生成する
	std::shared_ptr<ColliderBase> CreateColliderFromSlot(
		const ActorDataLoader::ColliderSlotBinaryData& slot,
		ColliderBase::TAG selfTag,
		int modelHandle)
	{
		std::shared_ptr<ColliderBase> collider = nullptr;
		ColliderBase::TAG actualSelfTag = (slot.selfTag >= 0)
			? static_cast<ColliderBase::TAG>(slot.selfTag)
			: selfTag;

		switch (static_cast<ColliderBase::SHAPE>(slot.shapeType))
		{
		case ColliderBase::SHAPE::LINE:
			collider = std::make_shared<ColliderLine>(
				actualSelfTag, nullptr, slot.vectorParameter1, slot.vectorParameter2);
			break;

		case ColliderBase::SHAPE::SPHERE:
			collider = std::make_shared<ColliderSphere>(
				actualSelfTag, nullptr, slot.vectorParameter1, slot.radius);
			break;

		case ColliderBase::SHAPE::CAPSULE:
			collider = std::make_shared<ColliderCapsule>(
				actualSelfTag, nullptr, slot.vectorParameter1,
				slot.vectorParameter2, slot.radius);
			break;

		case ColliderBase::SHAPE::MODEL:
		{
			auto modelCollider = std::make_shared<ColliderModel>(actualSelfTag, nullptr);
			if (modelHandle != -1)
			{
				modelCollider->SetModelHandle(modelHandle);
			}
			collider = modelCollider;
		}
		break;

		case ColliderBase::SHAPE::BOX:
			collider = std::make_shared<ColliderBox>(
				actualSelfTag, nullptr, slot.vectorParameter1, slot.vectorParameter2);
			break;

		default:
			break;
		}

		if (collider != nullptr)
		{
			collider->SetTrigger(slot.isTrigger != 0);

			// 標準のターゲットタグを設定
			if (selfTag == ColliderBase::TAG::PLAYER)
			{
				collider->AddTargetTag(ColliderBase::TAG::STAGE);
				collider->AddTargetTag(ColliderBase::TAG::WALL);
				collider->AddTargetTag(ColliderBase::TAG::ENEMY);
			}
			else if (selfTag == ColliderBase::TAG::ENEMY)
			{
				collider->AddTargetTag(ColliderBase::TAG::STAGE);
				collider->AddTargetTag(ColliderBase::TAG::WALL);
				collider->AddTargetTag(ColliderBase::TAG::PLAYER);
				collider->AddTargetTag(ColliderBase::TAG::ENEMY);
				collider->AddTargetTag(ColliderBase::TAG::PLAYER_SWORD);
			}
		}

		return collider;
	}
}

std::unordered_map<std::wstring, ActorFactory::RecipeFunc> ActorFactory::recipes_;

void ActorFactory::RegisterDefaultRecipes(void)
{
	RegisterRecipe(RECIPE_PLAYER, BuildPlayer);
	RegisterRecipe(RECIPE_ENEMY, BuildEnemy);
	RegisterRecipe(RECIPE_SWORD, BuildSword);
}

void ActorFactory::RegisterRecipe(const std::wstring& actorName, RecipeFunc recipe)
{
	recipes_[actorName] = recipe;
}

Entity ActorFactory::CreateActor(EcsRegistry& registry, const std::wstring& actorName)
{
	auto iterator = recipes_.find(actorName);
	if (iterator == recipes_.end())
	{
		return NULL_ENTITY;
	}

	return iterator->second(registry);
}

Entity ActorFactory::BuildPlayer(EcsRegistry& registry)
{
	ActorDataLoader::PlayerBinaryData playerParam{};                 // プレイヤーパラメータ
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + PLAYER_CSV_NAME;     // CSVパス
	std::wstring binaryPath = std::wstring(Application::PATH_BINARY) + PLAYER_BIN_NAME; // バイナリパス

	ActorDataLoader::LoadPlayerData(csvPath, binaryPath, playerParam);

	Entity playerEntity = registry.CreateEntity(); // プレイヤーエンティティ

	TransformComponent playerTransform{};
	playerTransform.scale = playerParam.scale;
	registry.AddComponent<TransformComponent>(playerEntity, playerTransform);

	VelocityComponent playerVelocity{};
	playerVelocity.maxSpeed = playerParam.maxSpeed;
	registry.AddComponent<VelocityComponent>(playerEntity, playerVelocity);

	PlayerInputComponent playerInput{};
	playerInput.moveSpeed = playerParam.moveSpeed;
	playerInput.jumpPower = playerParam.jumpPower;
	registry.AddComponent<PlayerInputComponent>(playerEntity, playerInput);

	ModelComponent modelComponent{};
	modelComponent.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_PLAYER);
	registry.AddComponent<ModelComponent>(playerEntity, modelComponent);

	StatusComponent playerStatus{};
	playerStatus.maxHp = playerParam.maxHp;
	playerStatus.currentHp = playerParam.maxHp;
	playerStatus.attackPower = playerParam.attackPower;
	playerStatus.defensePower = playerParam.defensePower;
	playerStatus.nextLevelExperience = playerParam.nextLevelExperience;
	playerStatus.growthHp = playerParam.growthHp;
	playerStatus.growthAttack = playerParam.growthAttack;
	playerStatus.growthDefense = playerParam.growthDefense;
	registry.AddComponent<StatusComponent>(playerEntity, playerStatus);

	// 汎用コライダーの構築
	CollisionComponent collisionComponent{};
	for (int index = 0; index < playerParam.colliderCount; ++index)
	{
		auto collider = CreateColliderFromSlot(
			playerParam.colliders[index],
			ColliderBase::TAG::PLAYER,
			modelComponent.modelHandle
		);
		if (collider != nullptr)
		{
			collisionComponent.colliders.push_back(collider);
		}
	}
	registry.AddComponent<CollisionComponent>(playerEntity, collisionComponent);

	registry.AddComponent<AnimationComponent>(playerEntity, AnimationComponent{});

	int handleMove = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_RUN");
	int handleJump = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_JUMP");
	int handleAttack = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_ATTACK");

	AnimationSystem::AddInternal(
		registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::IDLE), 0, 60.0f);
	AnimationSystem::AddExternal(
		registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::MOVE), handleMove, 60.0f);
	AnimationSystem::AddExternal(
		registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::JUMP), handleJump, 30.0f);
	AnimationSystem::AddExternal(
		registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::ATTACK), handleAttack, 60.0f);

	// 初期状態はIDLEをループ再生
	AnimationSystem::Play(
		registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::IDLE), true, 60.0f);

#ifdef _DEBUG
	DebugGuiComponent playerGui{};
	playerGui.windowTitle = "Player";
	playerGui.saveType = DEBUG_SAVE_TYPE::PLAYER;
	playerGui.csvFilePath = csvPath;
	playerGui.binaryFilePath = binaryPath;
	registry.AddComponent<DebugGuiComponent>(playerEntity, playerGui);
#endif // _DEBUG

	return playerEntity;
}

Entity ActorFactory::BuildSword(EcsRegistry& registry)
{
	Entity swordEntity = registry.CreateEntity(); // 剣エンティティ

	TransformComponent transform{};
	transform.rotationEuler = VGet(
		UtilityMath::Deg2RadF(180.0f), // X軸の角度
		UtilityMath::Deg2RadF(0.0f),  // Y軸の角度
		UtilityMath::Deg2RadF(0.0f)   // Z軸の角度
	);
	transform.rotation_ = Quaternion::CreateEuler(
		transform.rotationEuler.x,
		transform.rotationEuler.y,
		transform.rotationEuler.z
	);
	transform.scale = VGet(1.0f, 1.0f, 1.0f);
	registry.AddComponent<TransformComponent>(swordEntity, transform);

	ModelComponent modelComponent{};
	modelComponent.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_SWORD);
	registry.AddComponent<ModelComponent>(swordEntity, modelComponent);

	HierarchyComponent hierarchy{};
	hierarchy.attachBoneName = L"mixamorig:RightHandIndex2";
	registry.AddComponent<HierarchyComponent>(swordEntity, hierarchy);

	// カプセルコライダーを剣タグで作成
	auto swordCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER_SWORD,
		nullptr,
		VGet(0.0f, 0.0f, 0.0f),
		VGet(0.0f, 50.0f, 0.0f),
		5.0f
	);
	swordCapsule->AddTargetTag(ColliderBase::TAG::ENEMY);

	CollisionComponent collisionComponent{};
	collisionComponent.colliders.push_back(swordCapsule);
	registry.AddComponent<CollisionComponent>(swordEntity, collisionComponent);

#ifdef _DEBUG
	DebugGuiComponent swordGui{};
	swordGui.windowTitle = "Sword";
	swordGui.saveType = DEBUG_SAVE_TYPE::NONE;
	registry.AddComponent<DebugGuiComponent>(swordEntity, swordGui);
#endif // _DEBUG

	return swordEntity;
}

Entity ActorFactory::BuildEnemy(EcsRegistry& registry)
{
	ActorDataLoader::EnemyBinaryData enemyParam{};                 // エネミーパラメータ
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + ENEMY_CSV_NAME;     // CSVパス
	std::wstring binaryPath = std::wstring(Application::PATH_BINARY) + ENEMY_BIN_NAME; // バイナリパス

	ActorDataLoader::LoadEnemyData(csvPath, binaryPath, enemyParam);

	Entity enemyEntity = registry.CreateEntity(); // エネミーエンティティ

	TransformComponent transform{};
	transform.scale = enemyParam.scale;
	registry.AddComponent<TransformComponent>(enemyEntity, transform);

	VelocityComponent velocity{};
	registry.AddComponent<VelocityComponent>(enemyEntity, velocity);

	registry.AddComponent<EnemyComponent>(enemyEntity, EnemyComponent{});

	ModelComponent modelComponent{};
	modelComponent.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_ENEMY);
	registry.AddComponent<ModelComponent>(enemyEntity, modelComponent);

	StatusComponent enemyStatus{};
	enemyStatus.maxHp = enemyParam.maxHp;
	enemyStatus.currentHp = enemyParam.maxHp;
	enemyStatus.attackPower = enemyParam.attackPower;
	enemyStatus.defensePower = enemyParam.defensePower;
	enemyStatus.rewardExperience = enemyParam.rewardExperience;
	registry.AddComponent<StatusComponent>(enemyEntity, enemyStatus);

	// 汎用コライダーの構築
	CollisionComponent collisionComponent{};
	for (int index = 0; index < enemyParam.colliderCount; ++index)
	{
		auto collider = CreateColliderFromSlot(
			enemyParam.colliders[index],
			ColliderBase::TAG::ENEMY,
			modelComponent.modelHandle
		);
		if (collider != nullptr)
		{
			collisionComponent.colliders.push_back(collider);
		}
	}
	registry.AddComponent<CollisionComponent>(enemyEntity, collisionComponent);

#ifdef _DEBUG
	DebugGuiComponent enemyGui{};
	enemyGui.windowTitle = "Enemy_" + std::to_string(static_cast<int>(enemyEntity));
	enemyGui.saveType = DEBUG_SAVE_TYPE::ENEMY;
	enemyGui.csvFilePath = csvPath;
	enemyGui.binaryFilePath = binaryPath;
	registry.AddComponent<DebugGuiComponent>(enemyEntity, enemyGui);
#endif // _DEBUG

	return enemyEntity;
}

Entity ActorFactory::CreateCamera(EcsRegistry& registry, Entity targetEntity)
{
	ActorDataLoader::CameraBinaryData cameraParam{};                 // カメラパラメータ
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + CAMERA_CSV_NAME;     // CSVパス
	std::wstring binaryPath = std::wstring(Application::PATH_BINARY) + CAMERA_BIN_NAME; // バイナリパス

	ActorDataLoader::LoadCameraData(csvPath, binaryPath, cameraParam);

	Entity cameraEntity = registry.CreateEntity(); // カメラエンティティ
	registry.AddComponent<TransformComponent>(cameraEntity, TransformComponent{});

	CameraComponent cameraData{};
	cameraData.targetEntity = targetEntity;
	cameraData.mode = CAMERA_MODE::TRACKING;
	cameraData.targetOffset = cameraParam.targetOffset;
	cameraData.distance = cameraParam.distance;
	cameraData.angleH = cameraParam.angleHorizontal;
	cameraData.angleV = cameraParam.angleVertical;
	cameraData.rotationSpeed = cameraParam.rotationSpeed;
	cameraData.freeMoveSpeed = cameraParam.freeMoveSpeed;
	cameraData.fov = cameraParam.fov;
	cameraData.nearZ = cameraParam.nearZ;
	cameraData.farZ = cameraParam.farZ;
	registry.AddComponent<CameraComponent>(cameraEntity, cameraData);

	// カメラの汎用コライダー構築
	CollisionComponent collisionComponent{};
	for (int index = 0; index < cameraParam.colliderCount; ++index)
	{
		auto collider = CreateColliderFromSlot(
			cameraParam.colliders[index],
			ColliderBase::TAG::WALL,
			-1
		);
		if (collider != nullptr)
		{
			collisionComponent.colliders.push_back(collider);
		}
	}
	if (cameraParam.colliderCount > 0)
	{
		registry.AddComponent<CollisionComponent>(cameraEntity, collisionComponent);
	}

#ifdef _DEBUG
	DebugGuiComponent cameraGui{};
	cameraGui.windowTitle = "Camera";
	cameraGui.saveType = DEBUG_SAVE_TYPE::CAMERA;
	cameraGui.csvFilePath = csvPath;
	cameraGui.binaryFilePath = binaryPath;
	registry.AddComponent<DebugGuiComponent>(cameraEntity, cameraGui);
#endif // _DEBUG

	return cameraEntity;
}