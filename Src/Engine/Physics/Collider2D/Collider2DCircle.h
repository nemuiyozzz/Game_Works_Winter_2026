#pragma once
#include "Collider2DBase.h"
#include "../../../Common/Vector2.h"

/// @brief 2D円コライダーのクラス
class Collider2DCircle : public Collider2DBase
{
public:

	/// @brief コンストラクタ
	/// @param centerPos 初期の中央座標
	/// @param radius 円の半径
	/// @param _collisionTag 衝突の2D識別タグ
	Collider2DCircle(const Vector2F& centerPos, float radius, Collider2DBase::TAG_2D collisionTag);

	/// @brief デストラクタ
	virtual ~Collider2DCircle(void) override = default;

	/// @brief 中心座標を直接変更する
	/// @param centerPos 新しい中心座標
	void SetCenterPos(const Vector2F& centerPos);

	/// @brief 半径の変更
	/// @param radius 新しい半径
	void SetRadius(float radius);

	/// @brief 中央座標を取得
	/// @return 中央座標
	Vector2F GetWorldCenterPos(void) const;

	/// @brief 半径の取得
	/// @return 半径
	float GetRadius(void) const;

protected:

	/// @brief デバッグ描画
	/// @param _color 描画色
	virtual void DrawDebug(int color) const override;

private:

	// 円の半径
	float radius_;

	// 中央座標
	Vector2F centerPos_;
};