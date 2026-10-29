#pragma once

#include <memory>

class PlayerActionFacade;

/// @brief プレイヤーステートの基底インターフェースクラス
class IPlayerState
{
public:

	// プレイヤーのステート種別
	enum class STATE_ID
	{
		IDLE,         // 待機
		MOVE,         // 通常移動
		SPRINT,       // 疾走（ナイトレイン風ダッシュ）
		JUMP,         // ジャンプ（空中）
		JUMP_ATTACK,  // ジャンプ攻撃
		ATTACK_COMBO, // 地上コンボ攻撃
		EVADE         // 回避（無敵ステップ）
	};

	/// @brief コンストラクタ
	IPlayerState(void) = default;

	/// @brief 仮想デストラクタ
	virtual ~IPlayerState(void) = default;

	/// @brief ステート開始時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	virtual void EnterState(PlayerActionFacade& action) = 0;

	/// @brief ステート更新処理を行う
	/// @param action プレイヤーアクション窓口Facade
	/// @param deltaTime フレーム経過時間
	/// @return 遷移先のステート（継続時はnullptr）
	virtual std::shared_ptr<IPlayerState> UpdateState(
		PlayerActionFacade& action, float deltaTime) = 0;

	/// @brief ステート終了時の処理を行う
	/// @param action プレイヤーアクション窓口Facade
	virtual void ExitState(PlayerActionFacade& action) = 0;

	/// @brief ステートIDを取得する
	/// @return ステート種別ID
	virtual STATE_ID GetStateId(void) const = 0;
};