#pragma once
#include <Vector>

enum class SOUND_TYPE
{
	NONE,
	BGM,
	SE,
};

enum class SOUND_ID
{
	// BGM
	NONE,
	BGM_TITLE,
	BGM_STAGE,
	BGM_LOBBY,
	BGM_RESULT,

	// SE
	SE_UI_SELECT,
	SE_UI_CANCEL,
	SE_SWORD_SWING,
	SE_SWORD_HIT,
};

struct SoundRequest
{
	SOUND_ID soundId = SOUND_ID::NONE;
	bool is3D = false;
	float radius = 0.0f;
};

struct SoundComponent
{
	std::vector<SoundRequest> playRequests;

	void Play(SOUND_ID id)
	{
		playRequests.push_back({ id, false, 0.0f });
	}

	void Play3D(SOUND_ID id, float radius = 1000.0f)
	{
		playRequests.push_back({ id, true, radius });
	}
};