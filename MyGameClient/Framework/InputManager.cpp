#include "pch.h"
#include "Singleton.h"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include "InputManager.h"



void CInputManager::Init()
{
}

void CInputManager::Update()
{
	for (int32 i = 0; i < 256; ++i)
	{
		if (0x8000 & GetAsyncKeyState(i))
		{
			if (KEY_STATE::KEY_NONE == _keyBuffer[i] || KEY_STATE::KEY_UP == _keyBuffer[i])
			{
				_keyBuffer[i] = KEY_STATE::KEY_DOWN;
			}
			else
			{
				_keyBuffer[i] = KEY_STATE::KEY_PRESSED;
			}
		}
		else
		{
			if (KEY_STATE::KEY_PRESSED == _keyBuffer[i] || KEY_STATE::KEY_DOWN == _keyBuffer[i])
			{
				_keyBuffer[i] = KEY_STATE::KEY_UP;
			}
			else
			{
				_keyBuffer[i] = KEY_STATE::KEY_NONE;
			}
		}
	};
}

void CInputManager::SetKeyInputState(const KEY_TYPE& key, const KEY_STATE& key_state)
{
	_keyBuffer[(int32)key] = key_state;
}

KEY_STATE CInputManager::GetKeyInputState(const KEY_TYPE& key)
{
	return _keyBuffer[(int32)key];
}
