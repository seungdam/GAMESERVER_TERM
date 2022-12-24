#pragma once
#include "EXP_OVER.h"

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
class Msession {
    OVER_EXP _recv_over;
public:
    std::mutex _s_lock;
    S_STATE _state;
    std::atomic_bool   _is_active;      // 주위에 플레이어가 있는가?

    // 기본  스탯
    int _id;
    int _max_hp;
    int _hp;
    int _exp;
    int _level;
    
    SOCKET _socket;
    short   x, y;
    char   _name[NAME_SIZE];
    int      _prev_remain;
    std::unordered_set <int> _view_list;
    std::mutex   _vl;
    int      last_move_time;
    std::mutex   _ll;
public:
    Msession()
    {
        _id = -1;
        _socket = 0;
        x = y = 0;
        _name[0] = 0;
        _state = ST_FREE;
        _prev_remain = 0;
    }

    ~Msession() {}

    void do_recv()
    {
        DWORD recv_flag = 0;
        memset(&_recv_over._over, 0, sizeof(_recv_over._over));
        _recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
        _recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
        WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
            &_recv_over._over, 0);
    }

    void do_send(void* packet);
    void send_login_info_packet();
    void send_move_packet(int c_id, std::array<Msession,210000>& clients);
    void send_add_object_packet(int c_id , std::array<Msession, 210000>& clients);
    void send_chat_packet(int c_id, const char* mess);
    void send_remove_object_packet(int c_id);
    
};
