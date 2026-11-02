#include "../../Pch.h"
#include "ColliderDebugDrawSystem.h"
#include "../Component/CollisionComponent.h"
#include "../Component/Collision2DComponent.h"
#include "../Component/TransformComponent.h"
#include "../../Engine/System/Input/KeyConfInputManager.h"

void ColliderDebugDrawSystem::Update(void)
{

	// キーが押された瞬間だけフラグを反転させる
	if (KeyConfInputManager::GetInstance().IsTriggerDown(L"DEBUG_COLLIDER"))
	{
		isDebugDrawActive_ = !isDebugDrawActive_;
	}
}

void ColliderDebugDrawSystem::Draw(EcsRegistry& registry) const
{
	if (!isDebugDrawActive_)
	{
		return;
	}

	// Zバッファを無効にして、壁の裏にあるコライダーも透けて見えるようにする
	SetUseZBuffer3D(false);
	SetWriteZBuffer3D(false);

	auto entities3D = registry.GetEntitiesWith<TransformComponent, CollisionComponent>();
	for (Entity entity : entities3D)
	{
		const TransformComponent& transform =
			registry.GetComponent<TransformComponent>(entity);
		CollisionComponent& collision =
			registry.GetComponent<CollisionComponent>(entity);

		for (auto& collider : collision.colliders)
		{
			if (collider != nullptr)
			{
				collider->SetFollowTarget(&transform);
				collider->Draw();
			}
		}
	}

	// 2Dコライダーの描画
	auto entities2D = registry.GetEntitiesWith<Collision2DComponent>();
	for (Entity entity : entities2D)
	{
		const Collision2DComponent& collision2D = registry.GetComponent<Collision2DComponent>(entity);

		for (const auto& collider : collision2D.colliders)
		{
			// 2Dコライダーを赤色で描画
			collider->DrawDebug(0xff0000);
		}
	}

	// 描画設定を元に戻す
	SetUseZBuffer3D(true);
	SetWriteZBuffer3D(true);
}