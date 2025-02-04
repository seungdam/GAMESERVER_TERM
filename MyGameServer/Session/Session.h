#pragma once
#include "../IOCP/OverEx.h"


class Session 
{
private:
    OverEX  _rOver;
    SOCKET  _sock;
public:
    int32_t _prevRemainSize;
public:
    std::mutex _cLock; // client state lock
    std::mutex   _vwLock; // view list lock
    std::atomic_bool  _isActive;      // 주위에 플레이어가 있는가?
    std::unordered_set <int32_t> _vwList;
    s_state _state;
    

    // 기본  스탯
    int32_t _id,
            _maxHp,
            _hp,
            _exp,
            _level,
            lastMoveTime;
    int16_t     _x, _y;
    char        _name[constant::name_len];
private:
    
public:
    Session() = default;
    ~Session() = default;

    void Init(int32_t, SOCKET);

    void DoAsyncRecv();
    void DoAsyncSend(void* packet);

    void DoSendAccountPacket();
    void DoSendMovePacket(int32_t c_id, std::array<Session, constant::max_npc + constant::max_user>& clients);
    void DoSendAddObjectPacket(int32_t c_id , std::array<Session, constant::max_npc + constant::max_user>& clients);
    void DoSendChatPacket(int32_t c_id, const char* msg);
    void DoSendRemoveObjectPacket(int32_t c_id);
    
    void Dispose() {closesocket(_sock);}
};
