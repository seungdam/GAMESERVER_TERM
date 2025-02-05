#pragma once
#include "common.h"

class GameObject 
{
private:
	bool m_showing;
	sf::Sprite m_sprite;
	sf::Text m_name;
	sf::Text m_chat;
public:
	int m_id;
	short m_x, m_y;
	int m_max_hp, m_hp;
	int m_level;
	int m_exp;
	char name[100];
public:
	GameObject()
	{
		m_showing = false;
	}
	GameObject(int id, short x, short y,int max_hp , int hp, int level, int exp, const char* name, sf::Texture& t) {
		m_id = id;
		m_x = x;
		m_y = y;
		m_max_hp = max_hp;
		m_hp = hp;
		m_level = level;
		m_exp = exp;

		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
		set_name(name);
	}

	void a_move(int, int);
	void a_draw(sf::RenderWindow&);
	void hide();
	void show();
	void set_font(sf::Font& font) {
		m_name.setFont(font);
		m_chat.setFont(font);
	}
	void set_name(const char str[]);
};

