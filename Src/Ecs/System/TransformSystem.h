#pragma once

#include <DxLib.h>
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"
#include "../Component/TransformComponent.h"

/// @brief TransformComponentの計算や操作を処理するシステムクラス
class TransformSystem
{
public:

	/// @brief コンストラクタ
	TransformSystem(void) = default;

	/// @brief デストラクタ
	~TransformSystem(void) = default;

	/// @brief エンティティのTransform行列を更新する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	void UpdateTransform(EcsRegistry& registry, Entity entity);

	/// @brief 移動処理（現在の位置に加算）
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param moveVector 移動量ベクトル
	void TranslatePosition(EcsRegistry& registry, Entity entity, const VECTOR& moveVector);

	/// @brief 回転処理（現在の回転に加算）
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param rotation 追加する回転（クォータニオン）
	void RotateQuaternion(EcsRegistry& registry, Entity entity, const Quaternion& rotation);

	/// @brief スケールを一律に設定する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param targetScale 設定するスケール値
	void SetUniformScale(EcsRegistry& registry, Entity entity, float targetScale);

	/// @brief 前方方向を取得する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @return 前方ベクトル
	VECTOR GetForwardDirection(EcsRegistry& registry, Entity entity) const;

	/// @brief 任意方向を取得する
	/// @param registry ECSデータベース
	/// @param entity 対象のエンティティID
	/// @param baseDirection 基準となる方向ベクトル
	/// @return 計算後の方向ベクトル
	VECTOR GetTargetDirection(
		EcsRegistry& registry, Entity entity, const VECTOR& baseDirection) const;
};