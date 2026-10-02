#include "../../Pch.h"
#include "ModelSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/ModelComponent.h"

void ModelSystem::DrawModel(EcsRegistry& registry, Entity entity)
{
	// 座標情報とモデル情報の両方を持っていなければ描画しない
	if (!registry.HasComponent<TransformComponent>(entity) ||
		!registry.HasComponent<ModelComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	ModelComponent& model = registry.GetComponent<ModelComponent>(entity);

	// ハンドルが有効な場合のみ描画処理を行う
	if (model.modelHandle_ != -1)
	{
		// Transformで計算済みの行列をモデルに適用する
		MV1SetMatrix(model.modelHandle_, transform.combinedMatrix_);

		// 3Dモデルを描画する
		MV1DrawModel(model.modelHandle_);
	}
}
