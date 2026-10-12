#include "ColliderBox.h"
#include "../../../Ecs/Component/TransformComponent.h"

namespace
{
	// 直方体の頂点数
	constexpr int VERTEX_COUNT = 8;
}

ColliderBox::ColliderBox(TAG collisionTag, const TransformComponent* followTarget,
	const VECTOR& localCenter, const VECTOR& size)
	: ColliderBase(SHAPE::BOX, collisionTag, followTarget)
	, localCenter_(localCenter)
	, size_(size)
{
}

void ColliderBox::SetLocalCenter(const VECTOR& localCenter)
{
	localCenter_ = localCenter;
}

void ColliderBox::SetSize(const VECTOR& size)
{
	size_ = size;
}

VECTOR ColliderBox::GetWorldCenter(void) const
{
	return TransformLocalToWorld(localCenter_);
}

void ColliderBox::DrawDebug(int color) const
{
	VECTOR halfSize = VScale(size_, 0.5f);

	// ローカル座標での8つの頂点を定義
	VECTOR localVertices[VERTEX_COUNT] = 
	{
		VGet(localCenter_.x - halfSize.x, localCenter_.y + halfSize.y, localCenter_.z - halfSize.z),
		VGet(localCenter_.x + halfSize.x, localCenter_.y + halfSize.y, localCenter_.z - halfSize.z),
		VGet(localCenter_.x + halfSize.x, localCenter_.y - halfSize.y, localCenter_.z - halfSize.z),
		VGet(localCenter_.x - halfSize.x, localCenter_.y - halfSize.y, localCenter_.z - halfSize.z),

		VGet(localCenter_.x - halfSize.x, localCenter_.y + halfSize.y, localCenter_.z + halfSize.z),
		VGet(localCenter_.x + halfSize.x, localCenter_.y + halfSize.y, localCenter_.z + halfSize.z),
		VGet(localCenter_.x + halfSize.x, localCenter_.y - halfSize.y, localCenter_.z + halfSize.z),
		VGet(localCenter_.x - halfSize.x, localCenter_.y - halfSize.y, localCenter_.z + halfSize.z)
	};

	// 8つの頂点をそれぞれのワールド座標に変換
	VECTOR worldVertices[VERTEX_COUNT];
	for (int i = 0; i < VERTEX_COUNT; ++i)
	{
		worldVertices[i] = TransformLocalToWorld(localVertices[i]);
	}

	// 手前側の面
	DrawLine3D(worldVertices[0], worldVertices[1], color);
	DrawLine3D(worldVertices[1], worldVertices[2], color);
	DrawLine3D(worldVertices[2], worldVertices[3], color);
	DrawLine3D(worldVertices[3], worldVertices[0], color);

	// 奥側の面
	DrawLine3D(worldVertices[4], worldVertices[5], color);
	DrawLine3D(worldVertices[5], worldVertices[6], color);
	DrawLine3D(worldVertices[6], worldVertices[7], color);
	DrawLine3D(worldVertices[7], worldVertices[4], color);

	// 手前と奥を繋ぐ線
	DrawLine3D(worldVertices[0], worldVertices[4], color);
	DrawLine3D(worldVertices[1], worldVertices[5], color);
	DrawLine3D(worldVertices[2], worldVertices[6], color);
	DrawLine3D(worldVertices[3], worldVertices[7], color);
}