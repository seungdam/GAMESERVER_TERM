#include "OBJECT.h"
void OBJECT::show()
{
	m_showing = true;
}
void OBJECT::hide()
{
	m_showing = false;
}

void OBJECT::a_move(int x, int y) {
	m_x = x;
	m_y = y;
	m_sprite.setPosition(x * 16, y * 16);
	m_name.setPosition(sf::Vector2f(m_x * 16 - (int)((m_name.getString().getSize() * 10) / 2)
		, m_y * 16 - 15));
}

void OBJECT::a_draw(sf::RenderWindow& window) {
	if (m_showing) {
		window.draw(m_name);
		window.draw(m_sprite);
	}
}

void OBJECT::set_name(const char str[]) {
	std::string s = "";
	s += str;
	s += " LV";
	s += std::to_string(m_level);
	m_name.setString(s);
	m_name.setFillColor(sf::Color(255, 255, 255));
	m_name.setStyle(sf::Text::Bold);
	m_name.setCharacterSize(15);
	m_name.setPosition(sf::Vector2f(m_x * 16 - (int)(m_name.getString().getSize() * 15 / 2)
		, m_y * 16 - 15));
}