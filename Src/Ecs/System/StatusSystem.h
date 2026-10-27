#pragma once

#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Entity.h"

/// @brief ステータスの時間経過更新やダメージ・レベルアップ計算を行うシステム
class StatusSystem : public ISystem
{
public:

	/// @brief コンストラクタ
	StatusSystem(void) = default;

	/// @brief デストラクタ
	~StatusSystem(void) override = default;

	/// @brief 無敵時間などのタイマーを更新する
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief 攻撃側から防御側へダメージを与え、死亡したかを返す
	/// @param registry ECSデータベース
	/// @param attackerEntity 攻撃側のエンティティID
	/// @param targetEntity ダメージを受ける側のエンティティID
	/// @return ダメージによって対象のHPが0以下になったら true
	static bool ApplyDamage(
		EcsRegistry& registry, Entity attackerEntity, Entity targetEntity);

	/// @brief 指定エンティティに経験値を加算し、必要量に達していればレベルアップさせる
	/// @param registry ECSデータベース
	/// @param targetEntity 経験値を得るエンティティID
	/// @param experienceAmount 獲得する経験値量
	static void AddExperience(
		EcsRegistry& registry, Entity targetEntity, int experienceAmount);

	/// @brief 
	/// @param registry 
	/// @param targetEntity 
	/// @return 
	bool TryLevelUp(EcsRegistry& registry, Entity targetEntity);
};