#include "../../Pch.h"
#include "ImageSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/ImageComponent.h"

void ImageSystem::Update(EcsRegistry& registry)
{
	auto entities = registry.GetEntitiesWith<TransformComponent, ImageComponent>();

	for (Entity entity : entities)
	{
		DrawImage(registry, entity);
	}
}

void ImageSystem::DrawImage(EcsRegistry& registry, Entity entity)
{
	// 座標情報と画像情報の両方を持っていなければ描画しない
	if (!registry.HasComponent<TransformComponent>(entity) ||
		!registry.HasComponent<ImageComponent>(entity))
	{
		return;
	}

	TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
	ImageComponent& image = registry.GetComponent<ImageComponent>(entity);

	// ハンドルが有効な場合のみ描画処理を行う
	if (image.imageHandle != -1)
	{
		double drawScale = static_cast<double>(transform.scale.x);
		double drawAngle = static_cast<double>(transform.rotationEuler.z);

		DrawRotaGraph(
			static_cast<int>(transform.position.x),
			static_cast<int>(transform.position.y),
			drawScale,
			drawAngle,
			image.imageHandle,
			image.isTransparent ? true : false
		);
	}
}
