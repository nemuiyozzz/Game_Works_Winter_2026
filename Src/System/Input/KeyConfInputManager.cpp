#include "../../Pch.h"
#include "KeyConfInputManager.h"
#include "../../Utility/UtilityMath.h"
#include "../../Application.h"

KeyConfInputManager* KeyConfInputManager::instance_ = nullptr;

void KeyConfInputManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new KeyConfInputManager();
	}
}

KeyConfInputManager& KeyConfInputManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		CreateInstance();
	}
	return *instance_;
}

void KeyConfInputManager::DestroyInstance(void)
{
	delete instance_;
	instance_ = nullptr;
}

KeyConfInputManager::KeyConfInputManager(void)
	: mousePosition_(UtilityMath::VECTOR2_ZERO)
	, inputTable_()
	, currentInputState_()
	, previousInputState_()
	, stickInfo_{}
	, rStickSensitivity_{}
	, mouseSensitivity_{}
	, mouseWheelVol_(0)
	, usePadNo_(DX_INPUT_PAD1)
{
	InitInputTable();
	LoadInputTable();
	LoadSensitivitySettings();

	for (const auto& pair : inputTable_)
	{
		currentInputState_[pair.first] = false;
		previousInputState_[pair.first] = false;
	}
}

void KeyConfInputManager::InitInputTable(void)
{
	inputTable_.clear();

	inputTable_[L"UP"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_W},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_UP},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_UP},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_UP)}
	};

	inputTable_[L"DOWN"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_S},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_DOWN},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_DOWN},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_DOWN)}
	};

	inputTable_[L"LEFT"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_A},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_LEFT},
	  {INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_LEFT)}
	};

	inputTable_[L"RIGHT"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_D},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_RIGHT},
	  {INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_STICK_RIGHT)}
	};

	inputTable_[L"JUMP"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_SPACE},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_RETURN},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_A}
	};

	inputTable_[L"DODGE"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_LSHIFT},
		{INPUT_TYPE::JOYPAD, PAD_INPUT_C}
	};

	inputTable_[L"ATTACK_NORMAL"] =
	{
		{INPUT_TYPE::MOUSE, MOUSE_INPUT_LEFT},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_TRIGGER)}
	};

	inputTable_[L"ATTACK_SPECIAL"] =
	{
		{INPUT_TYPE::MOUSE, MOUSE_INPUT_RIGHT},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_TRIGGER)}
	};

	inputTable_[L"OK"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_SPACE},
	  {INPUT_TYPE::MOUSE, MOUSE_INPUT_LEFT},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_A}
	};

	inputTable_[L"CANCEL"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_ESCAPE},
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_DELETE},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_B}
	};

	inputTable_[L"DEBUG_CANCEL"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_BACK}
	};

	inputTable_[L"PAUSE"] =
	{
	  {INPUT_TYPE::KEY_BOARD, KEY_INPUT_ESCAPE},
	  {INPUT_TYPE::JOYPAD, PAD_INPUT_8}
	};

	inputTable_[L"LOCK_ON"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_E},
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_STICK_PUSH)}
	};

	inputTable_[L"TARGET_CHANGE_LEFT"] =
	{
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::LEFT_SHOULDER)}
	};

	inputTable_[L"TARGET_CHANGE_RIGHT"] =
	{
		{INPUT_TYPE::XINPUT_ANALOG, static_cast<unsigned int>(XINPUT_ANALOG_ID::RIGHT_SHOULDER)}
	};

	inputTable_[L"CAMERA_UP"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_UP}
	};

	inputTable_[L"CAMERA_DOWN"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_DOWN}
	};

	inputTable_[L"CAMERA_LEFT"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_LEFT}
	};

	inputTable_[L"CAMERA_RIGHT"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_RIGHT}
	};

	inputTable_[L"APPLY_DEBUG"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_BACK},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_END}
	};

	inputTable_[L"UNAPPLY_DEBUG"] =
	{
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_RSHIFT},
		{INPUT_TYPE::KEY_BOARD, KEY_INPUT_HOME}
	};
}

void KeyConfInputManager::Update(void)
{
	previousInputState_ = currentInputState_;

	// キーボードの生データ
	std::array<char, 256> keyState{};
	GetHitKeyStateAll(keyState.data());

	int mouseState = GetMouseInput();
	int padState = GetJoypadInputState(usePadNo_);

	GetMousePoint(&mousePosition_.x, &mousePosition_.y);

	mouseWheelVol_ = GetMouseWheelRotVol();

	// XInputの状態
	XINPUT_STATE xInput{};
	if (GetJoypadXInputState(usePadNo_, &xInput) == 0)
	{
		stickInfo_.leftStickX = xInput.ThumbLX;
		stickInfo_.leftStickY = xInput.ThumbLY;
		stickInfo_.rightStickX = xInput.ThumbRX;
		stickInfo_.rightStickY = xInput.ThumbRY;
	}
	else
	{
		stickInfo_.leftStickX = 0;
		stickInfo_.leftStickY = 0;
		stickInfo_.rightStickX = 0;
		stickInfo_.rightStickY = 0;
	}

	for (const auto& pair : inputTable_)
	{
		const auto& eventName = pair.first;
		bool hit = false;

		if (eventName == L"APPLY_DEBUG" || eventName == L"UNAPPLY_DEBUG")
		{
			hit = true;

			for (const auto& inputInfo : pair.second)
			{
				if (CheckKeyboardInput(inputInfo, keyState) == false)
				{
					hit = false;
					break;
				}
			}
		}
		else
		{
			for (const auto& inputInfo : pair.second)
			{
				switch (inputInfo.type)
				{
				case INPUT_TYPE::KEY_BOARD:
				{
					if (CheckKeyboardInput(inputInfo, keyState))
					{
						hit = true;
					}
				}
				break;

				case INPUT_TYPE::MOUSE:
				{
					if ((mouseState & inputInfo.id) != 0)
					{
						hit = true;
					}
				}
				break;

				case INPUT_TYPE::JOYPAD:
				{
					if ((padState & inputInfo.id) != 0)
					{
						hit = true;
					}
				}
				break;

				case INPUT_TYPE::XINPUT_ANALOG:
				{
					if (CheckXInputAnalog(inputInfo, xInput))
					{
						hit = true;
					}
				}
				break;
				}
			}
		}
		currentInputState_[eventName] = hit;
	}
}

bool KeyConfInputManager::isPressed(const std::wstring& _name) const
{
	auto input = currentInputState_.find(_name);

	return (input != currentInputState_.end()) && input->second;
}

bool KeyConfInputManager::isTrigerUp(const std::wstring& _name) const
{
	auto curInput = currentInputState_.find(_name);
	auto preInput = previousInputState_.find(_name);

	if (curInput == currentInputState_.end())
	{
		return false;
	}

	return !(curInput->second) && preInput->second;
}

bool KeyConfInputManager::isTrigerDown(const std::wstring& _name) const
{
	auto curInput = currentInputState_.find(_name);
	auto preInput = previousInputState_.find(_name);

	if (curInput == currentInputState_.cend())
	{
		return false;
	}

	return curInput->second && !(preInput->second);
}

Vector2 KeyConfInputManager::GetMousePosition(void) const
{
	return mousePosition_;
}

Vector2F KeyConfInputManager::GetMouseVelocityAndFixCenter(void)
{
	int centerX = Application::SCREEN_HALF_X;
	int centerY = Application::SCREEN_HALF_Y;

	int currentX = 0; // 現在のマウスX
	int currentY = 0; // 現在のマウスY
	GetMousePoint(&currentX, &currentY);

	float diffX = static_cast<float>(currentX - centerX); // Xの移動量
	float diffY = static_cast<float>(currentY - centerY); // Yの移動量

	if (abs(diffX) <= 1.0f)
	{
		diffX = 0.0f;
	}

	if (abs(diffY) <= 1.0f)
	{
		diffY = 0.0f;
	}

	SetMousePoint(centerX, centerY);

	SetMouseDispFlag(false);

	return Vector2F(diffX, diffY);
}

void KeyConfInputManager::SetMouseSensitivity(const MouseSensitivity& _sensitivity)
{
	mouseSensitivity_ = _sensitivity;
}

const KeyConfInputManager::MouseSensitivity& KeyConfInputManager::GetMouseSensitivity(void) const
{
	return mouseSensitivity_;
}

void KeyConfInputManager::CalculateNormalizedStick(int _rawX, int _rawY, float _deadZone,
	float& _outNormalX, float& _outNormalY) const
{
	float normalX = static_cast<float>(_rawX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(_rawY) / XINPUT_VAL_MAX;
	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < _deadZone)
	{
		_outNormalX = 0.0f;
		_outNormalY = 0.0f;
		return;
	}

	float scale = (length - _deadZone) / (1.0f - _deadZone);

	_outNormalX = (normalX / length) * scale;
	_outNormalY = (normalY / length) * scale;
}

void KeyConfInputManager::ApplyRightStickSensitivity(int _rightStickX, int _rightStickY,
	float& _outNormalX, float& _outNormalY) const
{
	float normalX = 0.0f;
	float normalY = 0.0f;
	CalculateNormalizedStick(_rightStickX, _rightStickY, rStickSensitivity_.deadZone,
		normalX, normalY);

	normalX *= rStickSensitivity_.x;
	normalY *= rStickSensitivity_.y;

	normalX = std::fmax(-1.0f, std::fmin(1.0f, normalX));
	normalY = std::fmax(-1.0f, std::fmin(1.0f, normalY));

	if (rStickSensitivity_.invertX)
	{
		normalX = -normalX;
	}
	if (rStickSensitivity_.invertY)
	{
		normalY = -normalY;
	}

	_outNormalX = normalX;
	_outNormalY = normalY;
}

bool KeyConfInputManager::CheckKeyboardInput(const InputInfo& _inputInfo,
	const std::array<char, 256>& _keyState) const
{
	if (_keyState[_inputInfo.id] != 0)
	{
		return true;
	}
	return false;
}

bool KeyConfInputManager::CheckXInputAnalog(const InputInfo& _inputInfo,
	const XINPUT_STATE& _xInputState) const
{
	const int TRIGGER_THRESHOLD = 128; // トリガーのしきい値
	const int STICK_THRESHOLD = 16000; // スティックのしきい値

	switch (static_cast<XINPUT_ANALOG_ID>(_inputInfo.id))
	{
	case XINPUT_ANALOG_ID::LEFT_TRIGGER:
	{
		if (_xInputState.LeftTrigger > TRIGGER_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_TRIGGER:
	{
		if (_xInputState.RightTrigger > TRIGGER_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_STICK_UP:
	{
		if (_xInputState.ThumbLY > STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_STICK_DOWN:
	{
		if (_xInputState.ThumbLY < -STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_STICK_LEFT:
	{
		if (_xInputState.ThumbLX < -STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_STICK_RIGHT:
	{
		if (_xInputState.ThumbLX > STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_STICK_UP:
	{
		if (_xInputState.ThumbRY > STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_STICK_DOWN:
	{
		if (_xInputState.ThumbRY < -STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_STICK_LEFT:
	{
		if (_xInputState.ThumbRX < -STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_STICK_RIGHT:
	{
		if (_xInputState.ThumbRX > STICK_THRESHOLD)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_SHOULDER:
	{
		if (_xInputState.Buttons[XINPUT_BUTTON_LEFT_SHOULDER] != 0)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_SHOULDER:
	{
		if (_xInputState.Buttons[XINPUT_BUTTON_RIGHT_SHOULDER] != 0)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::LEFT_STICK_PUSH:
	{
		if (_xInputState.Buttons[XINPUT_BUTTON_LEFT_THUMB] != 0)
		{
			return true;
		}
	}
	break;

	case XINPUT_ANALOG_ID::RIGHT_STICK_PUSH:
	{
		if (_xInputState.Buttons[XINPUT_BUTTON_RIGHT_THUMB] != 0)
		{
			return true;
		}
	}
	break;

	}
	return false;
}

VECTOR KeyConfInputManager::GetLeftStickDirection(void) const
{
	float normalX = 0.0f;
	float normalY = 0.0f;

	CalculateNormalizedStick(stickInfo_.leftStickX, stickInfo_.leftStickY,
		LEFT_STICK_DEAD_ZONE, normalX, normalY);

	if (normalX == 0.0f && normalY == 0.0f)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}

	return VNorm(VGet(normalX, 0.0f, normalY));
}

Vector2F KeyConfInputManager::GetRIghtStick(void) const
{
	float x = 0.0f; // 処理後のX
	float y = 0.0f; // 処理後のY

	ApplyRightStickSensitivity(stickInfo_.rightStickX, stickInfo_.rightStickY, x, y);

	return Vector2F(x, y);
}

Vector2F KeyConfInputManager::GetLeftStickRaw(void) const
{
	float normalX = static_cast<float>(stickInfo_.leftStickX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(stickInfo_.leftStickY) / XINPUT_VAL_MAX;

	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < LEFT_STICK_DEAD_ZONE)
	{
		return UtilityMath::VECTOR2F_ZERO;
	}

	return Vector2F(normalX, normalY);
}

Vector2F KeyConfInputManager::GetRightStickRaw(void) const
{
	float normalX = static_cast<float>(stickInfo_.rightStickX) / XINPUT_VAL_MAX;
	float normalY = static_cast<float>(stickInfo_.rightStickY) / XINPUT_VAL_MAX;

	// スティックの傾きの長さ
	float length = sqrtf(normalX * normalX + normalY * normalY);

	if (length < rStickSensitivity_.deadZone)
	{
		return UtilityMath::VECTOR2F_ZERO;
	}

	return Vector2F(normalX, normalY);
}

void KeyConfInputManager::SetRStickSensitivity(const RStickSensitivity& _sensitivity)
{
	rStickSensitivity_ = _sensitivity;
}

const KeyConfInputManager::RStickSensitivity& KeyConfInputManager::GetRStickSensitivity(void) const
{
	return rStickSensitivity_;
}

void KeyConfInputManager::SaveInputTable(void)
{
	FILE* file = nullptr;
	_wfopen_s(&file, Application::PATH_KEY_CONFIG.c_str(), L"wb");

	if (!file)
	{
		return;
	}

	// ヘッダー情報
	KeyConfigHeader header{};
	memcpy(header.signature, "kcnf", 4);
	header.version = 1.0f;
	header.dataNum = static_cast<uint32_t>(inputTable_.size());
	fwrite(&header, sizeof(header), 1, file);

	for (const auto& pair : inputTable_)
	{
		// 名前の文字数
		uint8_t nameSize = static_cast<uint8_t>(pair.first.size());
		fwrite(&nameSize, sizeof(nameSize), 1, file);
		fwrite(pair.first.data(), sizeof(wchar_t), nameSize, file);

		// 割り当てられているキーの数
		uint8_t dataSize = static_cast<uint8_t>(pair.second.size());
		fwrite(&dataSize, sizeof(dataSize), 1, file);
		fwrite(pair.second.data(), sizeof(InputInfo), dataSize, file);
	}

	fclose(file);
}

void KeyConfInputManager::LoadInputTable(void)
{
	// ファイル読み込み
	auto handle = FileRead_open(Application::PATH_KEY_CONFIG.c_str());

	if (handle == 0)
	{
		return;
	}

	// ヘッダー情報
	KeyConfigHeader header{};
	FileRead_read(&header, sizeof(header), handle);

	if (memcmp(header.signature, "kcnf", 4) != 0)
	{
		FileRead_close(handle);
		return;
	}

	for (uint32_t i = 0; i < header.dataNum; ++i)
	{
		// 名前の文字数
		uint8_t nameSize = 0;

		FileRead_read(&nameSize, sizeof(nameSize), handle);

		// イベント名
		std::wstring eventName(nameSize, L'\0');
		FileRead_read(eventName.data(), nameSize * sizeof(wchar_t), handle);

		// 割り当てられているキーの数
		uint8_t dataSize = 0;
		FileRead_read(&dataSize, sizeof(dataSize), handle);

		inputTable_[eventName].resize(dataSize);
		FileRead_read(inputTable_[eventName].data(), sizeof(InputInfo) * dataSize, handle);
	}

	FileRead_close(handle);
}

void KeyConfInputManager::SaveSensitivitySettings(void) const
{
	FILE* file = nullptr;
	_wfopen_s(&file, Application::PATH_SENSITIVITY.c_str(), L"wb");

	if (!file)
	{
		return;
	}

	// 感度設定用のヘッダー
	SensitivityHeader header{};

	memcpy(header.signature, "sens", 4);
	fwrite(&header, sizeof(header), 1, file);
	fwrite(&mouseSensitivity_, sizeof(mouseSensitivity_), 1, file);
	fwrite(&rStickSensitivity_, sizeof(rStickSensitivity_), 1, file);

	fclose(file);
}

void KeyConfInputManager::LoadSensitivitySettings(void)
{
	// ファイル読み込み
	auto handle = FileRead_open(Application::PATH_SENSITIVITY.c_str());

	if (handle == 0)
	{
		return;
	}

	SensitivityHeader header{};

	FileRead_read(&header, sizeof(header), handle);

	if (memcmp(header.signature, "sens", 4) != 0)
	{
		FileRead_close(handle);
		return;
	}

	FileRead_read(&mouseSensitivity_, sizeof(mouseSensitivity_), handle);
	FileRead_read(&rStickSensitivity_, sizeof(rStickSensitivity_), handle);

	FileRead_close(handle);
}

void KeyConfInputManager::SetUsePadNo(int _padNo)
{
	usePadNo_ = _padNo;
}

int KeyConfInputManager::GetMouseWheel(void) const
{
	return mouseWheelVol_;
}