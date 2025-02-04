#include "../IOCP/OverEx.h"
#include "Session.h"


void Session::DoAsyncSend(void* packet)
{
    OverEX* sover = new OverEX{ reinterpret_cast<char*>(packet) };
    WSASend(_sock, &sover->_wsabuf, 1, 0, 0, static_cast<WSAOVERLAPPED*>(sover), 0);
}

void Session::Init(int32_t cId, SOCKET sock)
{
   _x = 0;
   _y = 0;
   _id = cId;
   _name[0] = 0;
   _prevRemainSize = 0;
   _sock = sock;
}

void Session::DoAsyncRecv()
{
    DWORD recvFlag = 0;
    ::ZeroMemory(reinterpret_cast<WSAOVERLAPPED*>(&_rOver), 0, sizeof(WSAOVERLAPPED));
    _rOver._wsabuf.len = constant::buf_len - _prevRemainSize;
    _rOver._wsabuf.buf = (_rOver._sendBuf) + _prevRemainSize;
    WSARecv(_sock, &_rOver._wsabuf, 1, 0, &recvFlag, reinterpret_cast<WSAOVERLAPPED*>(&_rOver), 0);
}

void Session::DoSendAccountPacket()
{
    SC_LOGIN_INFO_PACKET p;
    p.id = _id;
    p.size = sizeof(SC_LOGIN_INFO_PACKET);
    p.type = SC_LOGIN_INFO;
    p.x = _x;
    p.y = _y;
    p.hp = 100;
    p.level = 1;
    p.exp = 0;
    p.max_hp = 100;
    strcpy_s(p.name, _name);
    DoAsyncSend(&p);
}

void Session::DoSendMovePacket(int32_t targetId, std::array<Session, constant::max_npc + constant::max_user>& clients)
{
    SC_MOVE_OBJECT_PACKET p;
    p.id = targetId;
    p.size = sizeof(SC_MOVE_OBJECT_PACKET);
    p.type = SC_MOVE_OBJECT;
    p.x = _x;
    p.y = _y;
    p.move_time = clients[targetId].lastMoveTime;
    DoAsyncSend(&p);
}

void Session::DoSendAddObjectPacket(int32_t targetId, std::array<Session, constant::max_npc + constant::max_user>& clients)
{
    SC_ADD_OBJECT_PACKET packet;
    packet.id = targetId;
    strcpy_s(packet.name, clients[targetId]._name);
    packet.size = sizeof(packet);
    packet.type = SC_ADD_OBJECT;
    packet.x = clients[targetId]._x;
    packet.y = clients[targetId]._y;
    _vwLock.lock();
    _vwList.insert(targetId);
    _vwLock.unlock();
    DoAsyncSend(&packet);
}

void Session::DoSendChatPacket(int32_t targetId, const char* msg)
{
    auto packet = SC_CHAT_PACKET{};
    packet.id = targetId;
    packet.size = sizeof(packet);
    packet.type = SC_CHAT;
    strcpy_s(packet.msg, msg);
    DoAsyncSend(&packet);
}

void Session::DoSendRemoveObjectPacket(int32_t targetId)
{
    _vwLock.lock();
    if (_vwList.count(targetId))
    {
        _vwList.erase(targetId);
    }
    else
    {
        _vwLock.unlock();
        return;
    }
    _vwLock.unlock();
    SC_REMOVE_OBJECT_PACKET packet;
    packet.id = targetId;
    packet.size = sizeof(packet);
    packet.type = SC_REMOVE_OBJECT;
    DoAsyncSend(&packet);
}
