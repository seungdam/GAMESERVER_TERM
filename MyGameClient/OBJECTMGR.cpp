#include "OBJECTMGR.h"
void OBJECTMGR::AddObject(int id, short x, short y, int max_hp, int hp, int level, int exp, const char* name) {
	if (id < 10000) { // 플레이어라면
		if (m_pc_objects[id] == NULL) {
			m_pc_objects[id] = new GameObject(id, x, y, max_hp, hp, level, exp, name, m_player_texture);
			m_pc_objects[id]->a_move(x, y);
			m_pc_objects[id]->set_font(m_font);
			m_pc_objects[id]->show();
		}
		else {
			m_pc_objects[id]->show();
		}
	}
	else {
		if (m_pc_objects[id] == NULL) {
			m_pc_objects[id] = new GameObject(id, x, y, max_hp, hp, level, exp, name, m_npc_texture);
			m_pc_objects[id]->a_move(x, y);
			std::cout << "x" << x << "y" << y << std::endl;
			m_pc_objects[id]->set_font(m_font);
			m_pc_objects[id]->show();
		}
		else {
			m_pc_objects[id]->show();
		}
	}
}
void OBJECTMGR::RemoveObject(int id)
{
	if(m_pc_objects[id] != NULL)
		m_pc_objects[id]->hide();
}
void OBJECTMGR::Render(sf::RenderWindow& window) {
	for (int i = 0; i < 210000; ++i) {
		if (m_pc_objects[i] != NULL) {
			m_pc_objects[i]->a_draw(window);
		}
	}
}
void OBJECTMGR::Move(int id, int x, int y) {
	if (m_pc_objects[id] != NULL) {
		std::cout << x << " " << y << std::endl;
		m_pc_objects[id]->a_move(x, y);
	}
}