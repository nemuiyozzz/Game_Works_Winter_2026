#pragma once

/// @brief エンティティの戦闘ステータスやレベル情報を保持するコンポーネント
struct StatusComponent
{
	// 体力関連
	int currentHp = 100; // 現在の体力
	int maxHp = 100;     // 最大体力

	// 戦闘パラメータ関連
	int attackPower = 10; // 攻撃力
	int defensePower = 0; // 防御力

	// レベル・経験値関連
	int level = 1;                // 現在のレベル
	int currentExperience = 0;    // 現在の経験値
	int nextLevelExperience = 50; // 次のレベルまでに必要な経験値
	int rewardExperience = 20;    // 撃破時に相手に与える経験値

	// レベルアップ時の成長値関連
	int growthHp = 20;     // レベルアップ時の最大HP上昇量
	int growthAttack = 5;  // レベルアップ時の攻撃力上昇量
	int growthDefense = 2; // レベルアップ時の防御力上昇量

	// 無敵時間（連続ヒット防止）関連
	float invincibleTimer = 0.0f;    // 現在の無敵残り時間
	float invincibleDuration = 0.5f; // 被弾時の無敵発生時間（秒）
};