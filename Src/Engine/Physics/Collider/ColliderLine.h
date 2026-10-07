#pragma once

#include <DxLib.h>

#include "ColliderBase.h"

struct TransformComponent;

/// @brief 線分コライダクラス
class ColliderLine : public ColliderBase
{
public:

	/// @brief コンストラクタ
	/// @param collisionTag 衝突種別
	/// @param followTarget 追従対象のTransform
	/// @param localStartPos ローカル空間での開始位置
	/// @param localEndPos ローカル空間での終了位置
	ColliderLine(TAG collisionTag, const TransformComponent* followTarget, const VECTOR& localStartPos, const VECTOR& localEndPos);

	/// @brief デストラクタ
	~ColliderLine(void) override = default;

	/// @brief ローカル開始位置の設定
	/// @param _position ローカル空間での開始位置
	void SetLocalStartPos(const VECTOR& position);

	/// @brief ローカル終了位置の設定
	/// @param _position ローカル空間での終了位置
	void SetLocalEndPos(const VECTOR& position);

	/// @brief ローカル開始位置の取得
	/// @return ローカル空間での開始位置
	const VECTOR& GetLocalStartPos(void) const;

	/// @brief ローカル終了位置の取得
	/// @return ローカル空間での終了位置
	const VECTOR& GetLocalEndPos(void) const;

	/// @brief ワールド開始位置の取得
	/// @return ワールド空間での開始位置
	VECTOR GetWorldStartPos(void) const;

	/// @brief ワールド終了位置の取得
	/// @return ワールド空間での終了位置
	VECTOR GetWorldEndPos(void) const;

protected:

	/// @brief デバッグ描画
	/// @param debugColor 描画色
	void DrawDebug(int debugColor) const override;

private:

	// 定数関連
	static constexpr float DEBUG_POINT_RADIUS = 5.0f; // デバック表示の球体半径
	static constexpr int DEBUG_SEGMENT_COUNT = 6;     // デバック表示の球体ポリゴン分割数

	// コライダーの形状パラメータ関連
	VECTOR localStartPos_; // 線分の開始地点座標
	VECTOR localEndPos_;   // 線分の終了座標
};
