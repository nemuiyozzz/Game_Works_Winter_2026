#include "../../../Pch.h"
#include "ColliderSphere.h"
#include "../../../Ecs/Component/TransformComponent.h"

ColliderSphere::ColliderSphere(TAG collisionTag, const TransformComponent* followTarget,
    const VECTOR& localPos, float radius)
    : ColliderBase(SHAPE::SPHERE, collisionTag, followTarget)
    , localPos_(localPos)
    , radius_(radius)
{
}
void ColliderSphere::SetLocalPosition(const VECTOR& position)
{
    localPos_ = position;
}

void ColliderSphere::SetRadius(float radius)
{
    // 半径が負にならないように制限
    radius_ = std::max(radius, 0.0f);
}

const VECTOR& ColliderSphere::GetLocalPosition(void) const
{
    return localPos_;
}

VECTOR ColliderSphere::GetWorldPosition(void) const
{
    // ローカル座標をワールド座標へ変換
    return TransformLocalToWorld(localPos_);
}

float ColliderSphere::GetRadius(void) const
{
    return radius_;
}

void ColliderSphere::DrawDebug(int debugColor) const
{
    const VECTOR worldPosition = GetWorldPosition();

    // デバッグ用に球体を描画
    DrawSphere3D(worldPosition, radius_, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}
