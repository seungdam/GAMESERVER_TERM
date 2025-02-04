#include "../Header/pch.h"
#include "../Pattern/Singleton.h"
#include "EntityManager.h"

void CEntityManager::Init()
{
}

void CEntityManager::Update()
{
	for (auto& i : _gameObjs) i->Update();
}

void CEntityManager::Clear()
{
}

void CEntityManager::Reset()
{
}

void CEntityManager::Render(sf::RenderWindow& gameWindow)
{
	for (auto& i : _gameObjs) i->Render(gameWindow);
};
