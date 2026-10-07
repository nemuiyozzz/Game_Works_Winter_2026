#pragma once
#include "Collider2DBase.h"
#include "../../../Common/Vector2.h"

/// @brief 2D矩形コライダーのクラス
class Collider2DBox : public Collider2DBase
{
public:

	/// @brief コンストラクタ
	/// @param centerPos 初期の中央座標
	/// @param _width 矩形の横幅
	/// @param _height 矩形の縦幅
	/// @param _collisionTag 衝突の2D識別タグ
	Collider2DBox(const Vector2F& centerPos, float width, float height, TAG_2D collisionTag);

	/// @brief デストラクタ
	virtual ~Collider2DBox(void) override = default;

	/// @brief 中心座標の取得
	/// @return 中央座標
	Vector2F GetCenterPos(void) const;

	/// @brief 横幅の変更
	/// @param _width 新しい横幅
	void SetWidth(float width);

	/// @brief 縦幅の変更
	/// @param _height 新しい縦幅
	void SetHeight(float height);

	/// @brief ワールド空間上の中央座標を取得
	/// @return ワールド中央座標
	Vector2F GetWorldCenterPos(void) const;

	/// @brief 横幅の取得
	/// @return 矩形の横幅
	float GetWidth(void) const;

	/// @brief 縦幅の取得
	/// @return 矩形の縦幅
	float GetHeight(void) const;

	/// @brief 矩形の左上座標を取得
	/// @return 左上座標
	Vector2F GetLeftTop(void) const;

	/// @brief 矩形の右下座標を取得
	/// @return 右下座標
	Vector2F GetRightBottom(void) const;

	/// @brief デバッグ描画
	/// @param _color 描画色
	virtual void DrawDebug(int color) const override;

private:

	// 形状パラメータ関連
	float width_;            // 矩形の横幅
	float height_;           // 矩形の縦幅

	// 中央座標
	Vector2F centerPos_;
};