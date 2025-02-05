#pragma once
#include <array>
#include <random>
#include "common.h"
#include "GameObject.h"

class OBJECTMGR
{
	
	GameObject* m_pc_objects[210000];
	sf::Texture m_player_texture;
	sf::Texture m_npc_texture;
	sf::Font m_font;
public:
	OBJECTMGR() {
		for (int i = 0; i < 210000; ++i) {
			m_pc_objects[i] = NULL;
		}
		m_player_texture.loadFromFile("pixel16x16char.png", sf::IntRect(0, 0, 16, 16));
		m_npc_texture.loadFromFile("pixel16x16char.png", sf::IntRect(0, 72, 16, 16));
		m_font.loadFromFile("cour.ttf");
	
	};
	void AddObject(int id, short x, short y , int max_hp, int hp,
		int level, int exp, const char* name);
	GameObject& GetObject(int id) { return *m_pc_objects[id]; };
	void AddEXP(int id, int exp) { m_pc_objects[id]->m_exp += exp; }
	void RemoveObject(int id);
	void Render(sf::RenderWindow& window);
	void Move(int id, int x, int y);
	~OBJECTMGR() {};
};

