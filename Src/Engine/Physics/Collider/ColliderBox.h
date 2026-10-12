#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

struct TransformComponent;

class ColliderBox : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param collisionTag 衝突種別
	/// @param followTarget 追従対象のTransformComponent
	/// @param localCenter ローカル空間での中心座標
	/// @param size ボックスのサイズ（幅、高さ、奥行き）
	ColliderBox(TAG collisionTag, const TransformComponent* followTarget,
		const VECTOR& localCenter, const VECTOR& size);

	/// @brief デストラクタ 
	~ColliderBox(void) override = default;

	/// @brief ローカル中心座標を設定する
	/// @param localCenter 新しいローカル座標
	void SetLocalCenter(const VECTOR& localCenter);

	/// @brief サイズ（幅、高さ、奥行き）を設定する
	/// @param size 新しいサイズ
	void SetSize(const VECTOR& size);

	/// @brief ローカル中心座標を取得する
	/// @return ローカル中心座標
	const VECTOR& GetLocalCenter(void) const { return localCenter_; }

	/// @brief サイズを取得する
	/// @return ボックスのサイズ
	const VECTOR& GetSize(void) const { return size_; }

	/// @brief ワールド空間での中心座標を取得する
	/// @return ワールド中心座標
	VECTOR GetWorldCenter(void) const;

protected:

	/// @brief デバッグ描画
	/// @param color 描画色
	void DrawDebug(int color) const override;

private:

	// パラメータ関連
	VECTOR localCenter_; // ローカル空間での中心座標
	VECTOR size_;        // ボックスのサイズ（幅X、高さY、奥行きZ）
};

