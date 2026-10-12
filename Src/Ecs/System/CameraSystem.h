#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"

struct TransformComponent;
struct CameraComponent;

/// @brief CameraSystemのデータをもとにカメラを更新するシステム
class CameraSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	CameraSystem(void) = default;

	/// @brief デストラクタ 
	~CameraSystem(void) = default;

	/// @brief カメラを持つすべてのエンティティの座標を計算し、DxLibに適用する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

private:

	/// @brief 追従カメラの更新処理を行う
	/// @param registry ECSデータベース
	/// @param cameraTransform カメラのトランスフォーム
	/// @param camera カメラコンポーネント
	void UpdateTrackingCamera(EcsRegistry& registry, TransformComponent& cameraTransform,
		CameraComponent& camera);

	/// @brief フリーカメラの更新処理を行う
	/// @param cameraTransform カメラのトランスフォーム
	/// @param camera カメラコンポーネント
	void UpdateFreeCamera(TransformComponent& cameraTransform, CameraComponent& camera);
};