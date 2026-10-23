#include "../Pch.h"
#include "ActorFactory.h"
#include "EcsRegistry.h"
#include "../Application.h"
#include "../Engine/System/Resource/ResourceManager.h"
#include "../Engine/System/ExternalLoader/ActorDataLoader.h"
#include "../Engine/Physics/Collider/ColliderCapsule.h"
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
#include "../Engine/Physics/Collider/ColliderLine.h"
#include "System/AnimationSystem.h"
#include "System/PlayerControlSystem.h"

namespace
{
	// ファイル名関連
	constexpr const wchar_t* PLAYER_CSV_NAME = L"PlayerParam.csv"; // プレイヤーCSV名
	constexpr const wchar_t* PLAYER_BIN_NAME = L"PlayerParam.bin"; // プレイヤーバイナリ名
	constexpr const wchar_t* CAMERA_CSV_NAME = L"CameraParam.csv"; // カメラCSV名
	constexpr const wchar_t* CAMERA_BIN_NAME = L"CameraParam.bin"; // カメラバイナリ名

	// リソース・レシピキー名関連
	constexpr const wchar_t* KEY_MODEL_PLAYER = L"MODEL_PLAYER"; // プレイヤーモデル
	constexpr const wchar_t* RECIPE_PLAYER = L"Player";          // プレイヤーレシピ名
	constexpr const wchar_t* KEY_MODEL_ENEMY = L"MODEL_SAMPLE"; // エネミーモデル
	constexpr const wchar_t* RECIPE_ENEMY = L"Enemy";			 // エネミーレシピ名
	constexpr const wchar_t* KEY_MODEL_SWORD = L"MODEL_SWORD"; // 剣モデル
	constexpr const wchar_t* RECIPE_SWORD = L"Sword";          // 剣レシピ名
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
	PlayerBinaryData playerParam{};
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + PLAYER_CSV_NAME;
	std::wstring binPath = std::wstring(Application::PATH_BINARY) + PLAYER_BIN_NAME;

	ActorDataLoader::LoadPlayerData(csvPath, binPath, playerParam);

	Entity playerEntity = registry.CreateEntity();

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

	ModelComponent model{};
	model.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_PLAYER);
	registry.AddComponent<ModelComponent>(playerEntity, model);

	auto playerCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER,
		nullptr,
		playerParam.capsuleStartPos,
		playerParam.capsuleEndPos,
		playerParam.capsuleRadius
	);

	playerCapsule->AddTargetTag(ColliderBase::TAG::STAGE);
	playerCapsule->AddTargetTag(ColliderBase::TAG::WALL);
	playerCapsule->AddTargetTag(ColliderBase::TAG::ENEMY);

	auto playerRay = std::make_shared<ColliderLine>(
		ColliderBase::TAG::PLAYER,
		nullptr,
		VGet(0.0f, 50.0f, 0.0f), 
		VGet(0.0f, -5.0f, 0.0f) 
	);

	playerCapsule->AddTargetTag(ColliderBase::TAG::STAGE);
	playerCapsule->AddTargetTag(ColliderBase::TAG::WALL);

	CollisionComponent collisionComponent{};
	collisionComponent.colliders.push_back(playerCapsule);
	collisionComponent.colliders.push_back(playerRay);
	registry.AddComponent<CollisionComponent>(playerEntity, collisionComponent);

	registry.AddComponent<AnimationComponent>(playerEntity, AnimationComponent{});

	int handleMove = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_RUN");
	int handleJump = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_JUMP");
	int handleAttack = ResourceManager::GetInstance().GetHandleId(L"ANIM_PLAYER_ATTACK");

	AnimationSystem::AddInternal(registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::IDLE), 0,60.0f);
	AnimationSystem::AddExternal(registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::MOVE), handleMove, 60.0f);
	AnimationSystem::AddExternal(registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::JUMP), handleJump, 30.0f);
	AnimationSystem::AddExternal(registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::ATTACK), handleAttack, 60.0f);

	// 初期状態はIDLEをループ再生
	AnimationSystem::Play(registry, playerEntity, static_cast<int>(PlayerControlSystem::PLAYER_ANIM::IDLE), true, 60.0f);

#ifdef _DEBUG
	DebugGuiComponent playerGui{};
	playerGui.windowTitle = "Player";
	playerGui.saveType = DEBUG_SAVE_TYPE::PLAYER;
	playerGui.csvFilePath = csvPath;
	playerGui.binaryFilePath = binPath;
	registry.AddComponent<DebugGuiComponent>(playerEntity, playerGui);
#endif // _DEBUG

	return playerEntity;
}

Entity ActorFactory::BuildSword(EcsRegistry& registry)
{
	Entity swordEntity = registry.CreateEntity();

	TransformComponent transform{};
	transform.rotationEuler = VGet(
		UtilityMath::Deg2RadF(180.0f),  // X軸の角度
		UtilityMath::Deg2RadF(0.0f),  // Y軸の角度
		UtilityMath::Deg2RadF(0.0f)   // Z軸の角度
	);
	transform.rotation_ = Quaternion::Euler(
		transform.rotationEuler.x,
		transform.rotationEuler.y,
		transform.rotationEuler.z
	);
	transform.scale = VGet(1.0f, 1.0f, 1.0f);
	registry.AddComponent<TransformComponent>(swordEntity, transform);

	ModelComponent model{};
	model.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_SWORD);
	registry.AddComponent<ModelComponent>(swordEntity, model);

	HierarchyComponent hierarchy{};
	hierarchy.attachBoneName = L"mixamorig:RightHandIndex2";
	registry.AddComponent<HierarchyComponent>(swordEntity, hierarchy);

	// カプセルコライダーを剣タグで作成
	auto swordCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::PLAYER_SWORD,
		nullptr, // 親子関係で追従するためnullptrでOK
		VGet(0.0f, 0.0f, 0.0f),
		VGet(0.0f, 50.0f, 0.0f), // 剣の長さに合わせて調整
		5.0f
	);
	// 剣はエネミーに当たる
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
	Entity enemyEntity = registry.CreateEntity();

	TransformComponent transform{};
	transform.scale = VGet(30.0f, 30.0f, 30.0f);
	registry.AddComponent<TransformComponent>(enemyEntity, transform);

	VelocityComponent velocity{};
	registry.AddComponent<VelocityComponent>(enemyEntity, velocity);

	registry.AddComponent<EnemyComponent>(enemyEntity, EnemyComponent{});

	ModelComponent model{};
	model.modelHandle = ResourceManager::GetInstance().GetHandleId(KEY_MODEL_ENEMY);
	registry.AddComponent<ModelComponent>(enemyEntity, model);

	// カプセルコライダー
	auto enemyCapsule = std::make_shared<ColliderCapsule>(
		ColliderBase::TAG::ENEMY,
		nullptr,
		VGet(0.0f, 10.0f, 0.0f),
		VGet(0.0f, 30.0f, 0.0f),
		5.0f
	);
	enemyCapsule->AddTargetTag(ColliderBase::TAG::STAGE);
	enemyCapsule->AddTargetTag(ColliderBase::TAG::WALL);
	enemyCapsule->AddTargetTag(ColliderBase::TAG::PLAYER); 
	enemyCapsule->AddTargetTag(ColliderBase::TAG::ENEMY); 
	enemyCapsule->AddTargetTag(ColliderBase::TAG::PLAYER_SWORD); 

	// レイコライダー
	auto enemyRay = std::make_shared<ColliderLine>(
		ColliderBase::TAG::ENEMY,
		nullptr,
		VGet(0.0f, 50.0f, 0.0f),
		VGet(0.0f, -5.0f, 0.0f)
	);
	enemyRay->AddTargetTag(ColliderBase::TAG::STAGE);
	enemyRay->AddTargetTag(ColliderBase::TAG::WALL);

	CollisionComponent collisionComponent{};
	collisionComponent.colliders.push_back(enemyCapsule);
	collisionComponent.colliders.push_back(enemyRay);
	registry.AddComponent<CollisionComponent>(enemyEntity, collisionComponent);

	return enemyEntity;
}

Entity ActorFactory::CreateCamera(EcsRegistry& registry, Entity targetEntity)
{
	CameraBinaryData cameraParam{};
	std::wstring csvPath = std::wstring(Application::PATH_CSV) + CAMERA_CSV_NAME;
	std::wstring binPath = std::wstring(Application::PATH_BINARY) + CAMERA_BIN_NAME;

	ActorDataLoader::LoadCameraData(csvPath, binPath, cameraParam);

	Entity cameraEntity = registry.CreateEntity();
	registry.AddComponent<TransformComponent>(cameraEntity, {});

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

#ifdef _DEBUG
	DebugGuiComponent cameraGui{};
	cameraGui.windowTitle = "Camera";
	cameraGui.saveType = DEBUG_SAVE_TYPE::CAMERA;
	cameraGui.csvFilePath = csvPath;
	cameraGui.binaryFilePath = binPath;
	registry.AddComponent<DebugGuiComponent>(cameraEntity, cameraGui);
#endif // _DEBUG

	return cameraEntity;
}