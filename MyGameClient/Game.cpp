#include "Game.h"

void GAME::Tick()
{
}

void GAME::Render()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m_window->draw(m_map_sprite[i][j]);
		}
	}
	sf::RectangleShape hp;
	m_objectmgr->Render(*m_window);

	m_hp_bar.setPosition(m_view.getCenter().x - (m_exp_bar.getSize().x / 2), m_view.getCenter().y + 120);
	m_window->draw(m_hp_bar);
	m_max_hp_bar.setPosition(m_view.getCenter().x - (m_exp_bar.getSize().x / 2), m_view.getCenter().y + 120);
	m_window->draw(m_max_hp_bar);
	m_exp_bar.setPosition(m_view.getCenter().x - (m_exp_bar.getSize().x / 2), m_view.getCenter().y + 140);
	m_window->draw(m_exp_bar);
}

void GAME::LoadContent()
{
}
