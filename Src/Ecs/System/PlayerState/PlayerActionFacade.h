#pragma once

#include "../../EcsRegistry.h"
#include "../../Entity.h"

/// @brief プレイヤー関連のサブシステム（ECS・入力・アニメーション・物理）を隠蔽し、各ステートへ単一の窓口を提供するFacadeクラス
class PlayerActionFacade
{
public:

	// アクション別のスタミナ消費量関連
	static constexpr float COST_SPRINT_PER_SECOND = 15.0f; // 戦闘中ダッシュの秒間消費量
	static constexpr float COST_JUMP = 15.0f;              // ジャンプの消費量
	static constexpr float COST_JUMP_ATTACK = 25.0f;       // ジャンプ攻撃の消費量
	static constexpr float COST_ATTACK = 20.0f;            // 地上攻撃1段あたりの消費量
	static constexpr float COST_EVADE = 20.0f;             // 回避ステップの消費量

	/// @brief コンストラクタ
	/// @param registry ECSレジストリ参照
	/// @param entity 操作対象のプレイヤーエンティティ
	PlayerActionFacade(EcsRegistry& registry, Entity entity);

	/// @brief デストラクタ
	~PlayerActionFacade(void) = default;

	/// @brief 移動入力が行われているか確認する
	/// @return 移動入力があればtrue
	bool CheckMoveInput(void) const;

	/// @brief 疾走（ダッシュ）入力が保持されているか確認する
	/// @return 保持されていればtrue
	bool CheckSprintHeld(void) const;

	/// @brief ジャンプ入力がトリガーされたか確認する
	/// @return トリガーされていればtrue
	bool CheckJumpTriggered(void) const;

	/// @brief 攻撃入力がトリガーされたか確認する
	/// @return トリガーされていればtrue
	bool CheckAttackTriggered(void) const;

	/// @brief 回避入力がトリガーされたか確認する
	/// @return トリガーされていればtrue
	bool CheckEvadeTriggered(void) const;

	/// @brief 通常移動速度でカメラ基準の移動を行う
	/// @return 移動入力があり移動を実行したらtrue
	bool MoveNormal(void);

	/// @brief 疾走（ダッシュ）速度でカメラ基準の移動を行う
	/// @return 移動入力があり移動を実行したらtrue
	bool MoveSprint(void);

	/// @brief 空中での水平移動を行う（ダッシュ入力に応じて速度自動切替）
	void MoveInAir(void);

	/// @brief 回避ステップの初速を適用する（入力方向、無ければ正面方向）
	void ExecuteEvadeStep(void);

	/// @brief 攻撃時の前方踏み込み初速を適用する
	/// @param forwardSpeedRatio 通常移動速度に対する前方踏み込み速度の倍率
	void ExecuteForwardStep(float forwardSpeedRatio);

	/// @brief 水平方向（X, Z）の速度を指定倍率で減衰させる
	/// @param decelerationRate 減衰率（0.0〜1.0）
	void DecelerateHorizontalMovement(float decelerationRate);

	/// @brief 水平方向（X, Z）の移動を即座に停止させる
	void StopHorizontalMovement(void);

	/// @brief ジャンプの跳躍初速を適用する
	void ExecuteJumpVelocity(void);

	/// @brief 空中からの急降下速度を適用する
	/// @param downwardSpeedRatio ジャンプ力に対する急降下速度の倍率
	void ExecuteDownwardDive(float downwardSpeedRatio);

	/// @brief 地面に接地しているか確認する
	/// @return 接地していればtrue
	bool CheckGrounded(void) const;

	/// @brief スタミナを消費できるか判定し、可能なら消費する
	/// @param cost 消費するスタミナ量
	/// @return 消費に成功したらtrue
	bool TryConsumeStamina(float cost);

	/// @brief 戦闘中（周囲のエネミーに発見されている状態）か確認する
	/// @return 発見されていればtrue
	bool CheckInCombat(void) const;

	/// @brief 回避の無敵時間を設定する
	/// @param invincibleTime 設定する無敵秒数
	void SetInvincibleTimer(float invincibleTime);

	/// @brief 剣の攻撃判定コライダーの有効・無効を切り替える
	/// @param isActive 有効にするならtrue
	void SetSwordColliderActive(bool isActive);

	/// @brief 現在のステート経過時間を取得する
	/// @return 経過秒数
	float GetStateTimer(void) const;

	/// @brief コンボ段数と先行入力フラグを初期化する
	/// @param comboStep 設定するコンボ段数
	void SetupComboStep(int comboStep);

	/// @brief 次段コンボの先行入力フラグを記録する
	/// @param isBuffered 先行入力されたらtrue
	void SetNextComboBuffered(bool isBuffered);

	/// @brief 次段コンボが先行入力されているか確認する
	/// @return 先行入力されていればtrue
	bool CheckNextComboBuffered(void) const;

	/// @brief 現在再生中のアニメーションが終了したか確認する
	/// @return 終了していればtrue
	bool CheckAnimationEnded(void) const;

	/// @brief 待機アニメーションを再生する
	void PlayIdleAnimation(void);

	/// @brief 通常移動アニメーションを再生する
	void PlayMoveAnimation(void);

	/// @brief 疾走アニメーションを再生する
	void PlaySprintAnimation(void);

	/// @brief ジャンプアニメーションを再生する
	void PlayJumpAnimation(void);

	/// @brief 回避アニメーションを再生する
	void PlayEvadeAnimation(void);

	/// @brief 攻撃アニメーションを再生する
	void PlayAttackAnimation(void);

private:

	// サブシステム参照関連
	EcsRegistry& registry_; // ECSレジストリ参照
	Entity entity_;         // プレイヤーエンティティID

	/// @brief カメラの向きを基準とした移動速度と回転を適用する内部処理
	/// @param moveSpeed 適用する移動速度
	/// @return 移動入力があり速度を適用したらtrue
	bool ApplyCameraRelativeMovement(float moveSpeed);

	/// @brief 入力方向ベクトルを取得する内部処理
	/// @return 正規化された入力方向ベクトル
	VECTOR GetInputMoveDirection(void) const;
};