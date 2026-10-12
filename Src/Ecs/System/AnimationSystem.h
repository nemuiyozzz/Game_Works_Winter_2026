#pragma once

#include "ISystem.h"
#include "../../Ecs/EcsRegistry.h"
#include "../../Ecs/Entity.h"

/// @brief アニメーションの再生と更新を管理するシステム
class AnimationSystem : public ISystem
{
public:
	/// @brief コンストラクタ 
	AnimationSystem(void) = default;

	/// @brief デストラクタ 
	~AnimationSystem(void) override = default;

	/// @brief アニメーションコンポーネントを持つエンティティの更新
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief モデル内部のアニメーションを登録する
	static void AddInternal(EcsRegistry& registry, Entity entity, int type,
		int animIndex, float speed = 1.0f);

	/// @brief 外部モデルファイルからアニメーションを登録する
	static void AddExternal(EcsRegistry& registry, Entity entity, int type,
		int modelHandle, float speed = 1.0f);

	/// @brief 指定したタイプのアニメーションを再生する
	static void Play(EcsRegistry& registry, Entity entity, int type,
		bool isLoop, float blendTime);

	/// @brief アニメーションが終了しているか判定する
	static bool IsEnd(EcsRegistry& registry, Entity entity);

	/// @brief 特定のアニメーションが再生中かチェックする
	static bool IsPlaying(EcsRegistry& registry, Entity entity, int type);

	/// @brief 再生を一時停止する
	static void Pause(EcsRegistry& registry, Entity entity);

	/// @brief 一時停止を解除して再生を再開する
	static void Resume(EcsRegistry& registry, Entity entity);

	/// @brief 現在のアニメーションを特定の時間で停止させる
	static void PauseAtTime(EcsRegistry& registry, Entity entity, float time);

	/// @brief 現在のアニメーションを特定のフレーム数で停止させる
	static void PauseAtFrame(EcsRegistry& registry, Entity entity, int frameNumber);

	static int GetPlayType(EcsRegistry& registry, Entity entity);
	static int GetCurrentFrame(EcsRegistry& registry, Entity entity);
	static int GetTotalFrames(EcsRegistry& registry, Entity entity);
	static float GetCurrentTime(EcsRegistry& registry, Entity entity);
	static float GetTotalTime(EcsRegistry& registry, Entity entity);

private:

	/// @brief 単一のエンティティのアニメーションを更新する
	void UpdateAnimation(EcsRegistry& registry, Entity entity);
};