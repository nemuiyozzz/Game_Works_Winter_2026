#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "Entity.h"

class EcsRegistry;

/// @brief アクターの生成レシピを管理・実行するファサードクラス
class ActorFactory
{
public:

	// エンティティ生成関数の型定義
	using RecipeFunc = std::function<Entity(EcsRegistry&)>;

	/// @brief 標準のアクターレシピ（Playerなど）を一括登録する
	static void RegisterDefaultRecipes(void);

	/// @brief 新しいアクターの生成レシピを外部から追加登録する
	/// @param actorName アクターの識別名（例: L"EnemySlime"）
	/// @param recipe 生成処理を行う関数またはラムダ式
	static void RegisterRecipe(const std::wstring& actorName, RecipeFunc recipe);

	/// @brief 登録されたレシピ名をもとにアクターを生成する
	/// @param registry 登録先のECSレジストリ
	/// @param actorName 生成したいアクターの識別名
	/// @return 生成されたエンティティID
	static Entity CreateActor(EcsRegistry& registry, const std::wstring& actorName);

	/// @brief カメラエンティティを生成する
	/// @param registry 登録先のECSレジストリ
	/// @param targetEntity 追従対象となるエンティティID
	/// @return 生成されたカメラのエンティティID
	static Entity CreateCamera(EcsRegistry& registry, Entity targetEntity);

private:

	// 登録された生成レシピのマップ
	static std::unordered_map<std::wstring, RecipeFunc> recipes_;

	/// @brief インスタンス化禁止
	ActorFactory(void) = delete;
	~ActorFactory(void) = delete;

	/// @brief プレイヤーエンティティを生成する内部レシピ
	/// @param registry 登録先のECSレジストリ
	/// @return 生成されたプレイヤーのエンティティID
	static Entity BuildPlayer(EcsRegistry& registry);

	/// @brief 剣エンティティを生成する内部レシピ
	/// @param registry 登録先のECSレジストリ
	/// @return 生成された剣のエンティティID
	static Entity BuildSword(EcsRegistry& registry);

	/// @brief エネミーエンティティを生成する内部レシピ
	/// @param registry 登録先のECSレジストリ
	/// @return 生成されたエネミーのエンティティID
	static Entity BuildEnemy(EcsRegistry& registry);
};