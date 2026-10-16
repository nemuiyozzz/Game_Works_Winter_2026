#pragma once

#include "Entity.h"

class EcsRegistry;

/// @brief 各種アクター（エンティティ）の生成とコンポーネント付与をまとめるファサードクラス
class ActorFactory
{
public:

	/// @brief プレイヤーエンティティを生成し、必要なコンポーネントを付与する
	/// @param registry 登録先のECSレジストリ
	/// @return 生成されたプレイヤーのエンティティID
	static Entity CreatePlayer(EcsRegistry& registry);

	/// @brief カメラエンティティを生成し、必要なコンポーネントを付与する
	/// @param registry 登録先のECSレジストリ
	/// @param targetEntity 追従対象となるエンティティID
	/// @return 生成されたカメラのエンティティID
	static Entity CreateCamera(EcsRegistry& registry, Entity targetEntity);

private:

	/// @brief インスタンス化禁止
	ActorFactory(void) = delete;
	~ActorFactory(void) = delete;
};