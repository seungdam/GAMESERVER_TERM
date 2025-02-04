#pragma once
#include "../common.h"

enum op_type { OP_ACCEPT, OP_RECV, OP_SEND, OP_NPC_MOVE, OP_AI_HELLO };
class OverEX : public WSAOVERLAPPED
{
public:
    WSABUF _wsabuf;
    char _sendBuf[constant::buf_len];
    op_type _cmpType;
    int32_t _targetObjectId= 0;
    OverEX()
    {
        _wsabuf.len = constant::buf_len;
        _wsabuf.buf = _sendBuf;
        _cmpType = OP_RECV;
        ::ZeroMemory(static_cast<WSAOVERLAPPED*>(this), sizeof(WSAOVERLAPPED));
    }
    OverEX(char* packet)
    {
        _wsabuf.len = packet[0];
        _wsabuf.buf = _sendBuf;
        _cmpType = OP_SEND;
        ::memcpy(_sendBuf, packet, packet[0]);
        ::ZeroMemory(static_cast<WSAOVERLAPPED*>(this), sizeof(WSAOVERLAPPED));
    }
};

