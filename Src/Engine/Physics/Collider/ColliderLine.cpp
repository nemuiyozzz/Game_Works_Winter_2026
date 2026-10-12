#include "../../../Pch.h"
#include "ColliderLine.h"
#include "../../../Ecs/Component/TransformComponent.h"

ColliderLine::ColliderLine(TAG collisionTag, const TransformComponent* followTarget,
    const VECTOR& localStartPos, const VECTOR& localEndPos)
    : ColliderBase(SHAPE::LINE, collisionTag, followTarget)
    , localStartPos_(localStartPos)
    , localEndPos_(localEndPos)
{
}

void ColliderLine::SetLocalStartPos(const VECTOR& position)
{
    localStartPos_ = position;
}

void ColliderLine::SetLocalEndPos(const VECTOR& position)
{
    localEndPos_ = position;
}

const VECTOR& ColliderLine::GetLocalStartPos(void) const
{
    return localStartPos_;
}

const VECTOR& ColliderLine::GetLocalEndPos(void) const
{
    return localEndPos_;
}

VECTOR ColliderLine::GetWorldStartPos(void) const
{
    // ローカル座標 → ワールド座標へ変換
    return TransformLocalToWorld(localStartPos_);
}

VECTOR ColliderLine::GetWorldEndPos(void) const
{
    // ローカル座標 → ワールド座標へ変換
    return TransformLocalToWorld(localEndPos_);
}

void ColliderLine::DrawDebug(int debugColor) const
{
    const VECTOR WORLD_START_POS = GetWorldStartPos();
    const VECTOR WORLD_END_POS = GetWorldEndPos();

    // 線分を描画
    DrawLine3D(WORLD_START_POS, WORLD_END_POS, debugColor);

    // 開始点と終了点に分かりやすく球体を描画
    DrawSphere3D(WORLD_START_POS, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
    DrawSphere3D(WORLD_END_POS, DEBUG_POINT_RADIUS, DEBUG_SEGMENT_COUNT, debugColor, debugColor, false);
}
