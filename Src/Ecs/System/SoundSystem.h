#pragma once

#include <vector>
#include <mutex>
#include <DxLib.h>
#include "ISystem.h"
#include "../EcsRegistry.h"
#include "../Component/SoundComponent.h"

/// @brief サウンドリソースの管理と再生、3D音量計算を一括で行うシステム
class SoundSystem : public ISystem
{
public:

	/// @brief コンストラクタ 
	SoundSystem(void);

	/// @brief デストラクタ 
	~SoundSystem(void) override;

	/// @brief 各エンティティの再生リクエスト処理と3Dサウンドの更新
	/// @param registry ECSデータベース
	void Update(EcsRegistry& registry) override;

	/// @brief サウンドリソースの登録
	void AddSound(SOUND_TYPE type, SOUND_ID id, int dxHandle);

	/// @brief 指定サウンドの停止
	void Stop(SOUND_ID id);

	/// @brief 全BGMの停止
	void StopAllBGM(void);

	/// @brief BGMマスターボリュームの設定 (0-100)
	void SetMasterVolumeBGM(int volume);

	/// @brief SEマスターボリュームの設定 (0-100)
	void SetMasterVolumeSE(int volume);

	/// @brief サウンドが再生中か確認
	bool IsPlaying(SOUND_ID id);

private:

	/// @brief システム内部で保持するサウンドデータ構造体
	struct SoundData
	{
		SOUND_ID id;
		int handle;
		SOUND_TYPE type;
		int playMode;
		bool is3D;
		VECTOR playPos;
		float radius;
	};

	// 定数群
	static constexpr int MAX_PERCENT = 100;
	static constexpr int DX_MAX_VOLUME = 255;
	static constexpr int DEFAULT_BGM_VOLUME = 60;
	static constexpr int DEFAULT_SE_VOLUME = 90;

	// 管理変数
	std::vector<SoundData> sounds_;
	int masterVolumeBGM_;
	int masterVolumeSE_;
	std::recursive_mutex soundMutex_;

	// 内部処理用ヘルパー
	int ToDxVolume(int percent) const;
	void ApplyMasterVolumes(void);

};

