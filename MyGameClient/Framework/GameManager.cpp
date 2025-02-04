#include "pch.h"
#include "Singleton.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "SceneManager.h"
#include "GameManager.h"

void CGameManager::Init()
{
	CInputManager::getInstance().Init();
	CEntityManager::getInstance().Init();
	CSceneManager::getInstance().Init();
}

void CGameManager::Update()
{
	CInputManager::getInstance().Update();
	CEntityManager::getInstance().Update();
	CSceneManager::getInstance().Update();
}
