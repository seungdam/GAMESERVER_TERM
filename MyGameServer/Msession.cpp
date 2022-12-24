#include "Msession.h"


void Msession::do_send(void* packet)
{
    OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
    WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void Msession::send_login_info_packet()
{
    SC_LOGIN_INFO_PACKET p;
    p.id = _id;
    p.size = sizeof(SC_LOGIN_INFO_PACKET);
    p.type = SC_LOGIN_INFO;
    p.x = x;
    p.y = y;
    p.hp = 100;
    p.level = 1;
    p.exp = 0;
    p.max_hp = 100;
    strcpy_s(p.name, _name);
    do_send(&p);
}

void Msession::send_move_packet(int c_id, std::array<Msession, 210000>& clients)
{
    SC_MOVE_OBJECT_PACKET p;
    p.id = c_id;
    p.size = sizeof(SC_MOVE_OBJECT_PACKET);
    p.type = SC_MOVE_OBJECT;
    p.x = clients[c_id].x;
    p.y = clients[c_id].y;
    p.move_time = clients[c_id].last_move_time;
    do_send(&p);
}

void Msession::send_add_object_packet(int c_id, std::array<Msession, 210000>& clients)
{
    SC_ADD_OBJECT_PACKET add_packet;
    add_packet.id = c_id;
    strcpy_s(add_packet.name, clients[c_id]._name);
    add_packet.size = sizeof(add_packet);
    add_packet.type = SC_ADD_OBJECT;
    add_packet.x = clients[c_id].x;
    add_packet.y = clients[c_id].y;
    _vl.lock();
    _view_list.insert(c_id);
    _vl.unlock();
    do_send(&add_packet);
}

void Msession::send_chat_packet(int p_id, const char* mess)
{
    SC_CHAT_PACKET packet;
    packet.id = p_id;
    packet.size = sizeof(packet);
    packet.type = SC_CHAT;
    strcpy_s(packet.mess, mess);
    do_send(&packet);
}

void Msession::send_remove_object_packet(int c_id)
{
    _vl.lock();
    if (_view_list.count(c_id))
        _view_list.erase(c_id);
    else {
        _vl.unlock();
        return;
    }
    _vl.unlock();
    SC_REMOVE_OBJECT_PACKET p;
    p.id = c_id;
    p.size = sizeof(p);
    p.type = SC_REMOVE_OBJECT;
    do_send(&p);
}
