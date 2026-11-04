#include "../../Pch.h"
#include "SoundSystem.h"
#include "../Component/TransformComponent.h"
#include "../Component/CameraComponent.h"
#include "../../Engine/System/Resource/ResourceManager.h"

SoundSystem::SoundSystem(void)
	: masterVolumeBGM_(DEFAULT_BGM_VOLUME)
	, masterVolumeSE_(DEFAULT_SE_VOLUME)
{
	int swingHandle = ResourceManager::GetInstance().GetHandleId(L"SE_SWORD_SWING");
	if (swingHandle != -1) { AddSound(SOUND_TYPE::SE, SOUND_ID::SE_SWORD_SWING, swingHandle); }

	int hitHandle = ResourceManager::GetInstance().GetHandleId(L"SE_SWORD_HIT");
	if (hitHandle != -1) { AddSound(SOUND_TYPE::SE, SOUND_ID::SE_SWORD_HIT, hitHandle); }
}

SoundSystem::~SoundSystem(void)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);
	for (auto& sound : sounds_)
	{
		DeleteSoundMem(sound.handle);
	}
	sounds_.clear();
}

void SoundSystem::AddSound(SOUND_TYPE type, SOUND_ID id, int dxHandle)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);

	// 重複登録の防止
	auto it = std::find_if(sounds_.begin(), sounds_.end(),
		[id](const SoundData& s) { return s.id == id; });
	if (it != sounds_.end()) { return; }

	int mode = (type == SOUND_TYPE::BGM) ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	sounds_.push_back(SoundData{ id, dxHandle, type, mode, false, VGet(0,0,0), 0.0f });

	int targetVol = (type == SOUND_TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
	ChangeVolumeSoundMem(ToDxVolume(targetVol), dxHandle);
}

void SoundSystem::Update(EcsRegistry& registry)
{
	VECTOR listenerPos = VGet(0.0f, 0.0f, 0.0f);
	bool hasListener = false;

	// 聞き手となるカメラ座標の取得
	auto cameras = registry.GetEntitiesWith<TransformComponent, CameraComponent>();
	if (!cameras.empty())
	{
		listenerPos = registry.GetComponent<TransformComponent>(cameras.front()).position;
		hasListener = true;
	}

	std::lock_guard<std::recursive_mutex> lock(soundMutex_);

	if (hasListener)
	{
		for (auto& sound : sounds_)
		{
			if (sound.is3D && CheckSoundMem(sound.handle) == 1)
			{
				VECTOR diff = VSub(sound.playPos, listenerPos);
				float distance = VSize(diff);
				int volume = 0;
				if (distance <= sound.radius)
				{
					float ratio = 1.0f - (distance / sound.radius);
					volume = static_cast<int>(static_cast<float>(masterVolumeSE_) * ratio);
				}
				ChangeVolumeSoundMem(ToDxVolume(volume), sound.handle);
			}
		}
	}

	auto entities = registry.GetEntitiesWith<SoundComponent>();
	for (Entity entity : entities)
	{
		SoundComponent& soundComp = registry.GetComponent<SoundComponent>(entity);
		if (soundComp.playRequests.empty()) { continue; }

		VECTOR playPos = VGet(0.0f, 0.0f, 0.0f);
		bool hasTransform = registry.HasComponent<TransformComponent>(entity);
		if (hasTransform)
		{
			playPos = registry.GetComponent<TransformComponent>(entity).position;
		}

		for (const SoundRequest& req : soundComp.playRequests)
		{
			auto it = std::find_if(sounds_.begin(), sounds_.end(),
				[&req](const SoundData& s) { return s.id == req.soundId; });

			if (it != sounds_.end() && it->handle > 0)
			{
				if (req.is3D && hasTransform)
				{
					it->is3D = true;
					it->playPos = playPos;
					it->radius = req.radius;

					int volume = 0;
					if (hasListener)
					{
						VECTOR diff = VSub(playPos, listenerPos);
						float distance = VSize(diff);
						if (distance <= req.radius)
						{
							float ratio = 1.0f - (distance / req.radius);
							volume = static_cast<int>(static_cast<float>(masterVolumeSE_) * ratio);
						}
					}
					ChangeVolumeSoundMem(ToDxVolume(volume), it->handle);
				}
				else
				{
					it->is3D = false;
					int vol = (it->type == SOUND_TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
					ChangeVolumeSoundMem(ToDxVolume(vol), it->handle);
				}

				PlaySoundMem(it->handle, it->playMode);
			}
		}
		soundComp.playRequests.clear();
	}
}

void SoundSystem::Stop(SOUND_ID id)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);
	auto it = std::find_if(sounds_.begin(), sounds_.end(),
		[id](const SoundData& s) { return s.id == id; });

	if (it != sounds_.end())
	{
		StopSoundMem(it->handle);
	}
}

void SoundSystem::StopAllBGM(void)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);
	for (auto& sound : sounds_)
	{
		if (sound.type == SOUND_TYPE::BGM)
		{
			StopSoundMem(sound.handle);
		}
	}
}

void SoundSystem::SetMasterVolumeBGM(int volume)
{
	masterVolumeBGM_ = std::clamp(volume, 0, MAX_PERCENT);
	ApplyMasterVolumes();
}

void SoundSystem::SetMasterVolumeSE(int volume)
{
	masterVolumeSE_ = std::clamp(volume, 0, MAX_PERCENT);
	ApplyMasterVolumes();
}

bool SoundSystem::IsPlaying(SOUND_ID id)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);
	auto it = std::find_if(sounds_.begin(), sounds_.end(),
		[id](const SoundData& s) { return s.id == id; });

	if (it == sounds_.end()) { return false; }
	return CheckSoundMem(it->handle) == 1;
}

int SoundSystem::ToDxVolume(int percent) const
{
	return percent * DX_MAX_VOLUME / MAX_PERCENT;
}

void SoundSystem::ApplyMasterVolumes(void)
{
	std::lock_guard<std::recursive_mutex> lock(soundMutex_);
	for (auto& sound : sounds_)
	{
		int targetPercent = (sound.type == SOUND_TYPE::BGM) ? masterVolumeBGM_ : masterVolumeSE_;
		ChangeVolumeSoundMem(ToDxVolume(targetPercent), sound.handle);
	}
}