#include "Session/Session.h"
#include "DB/OBDCManager.h"
#include <cstring>

using namespace std;

HANDLE hIocp;
array<Session, constant::max_user + constant::max_npc> clients;

SOCKET glServerSock, glClientSock;
OverEX glAcceptOver;

inline bool IsPlayer(int object_id)
{
    return object_id < constant::max_user;
}

inline bool IsNpc(int object_id)
{
    return !IsPlayer(object_id);
}

inline bool IsVisible(int from, int to)
{
    if (abs(clients[from]._x - clients[to]._x) > constant::view_range) return false;
    return abs(clients[from]._y - clients[to]._y) <= constant::view_range;
}

inline bool IsDetected(int from, int to) 
{
    if (abs(clients[from]._x - clients[to]._x) > 1) return false;
    return abs(clients[from]._y - clients[to]._y) <= 1;
}


int32_t GetNewClientId()
{
    for (auto i = 0; i < constant::max_user; ++i) 
    {
        lock_guard <mutex> ll { clients[i]._cLock };
        if (clients[i]._state == s_state::ST_FREE)
        {
            return i;
        }
    }
    return -1;
}

void Disconnect(int32_t id)
{
    clients[id]._vwLock.lock();
    auto vwList = clients[id]._vwList;
    clients[id]._vwLock.unlock();
    for (auto& viewerId : vwList)
    {
        if (IsNpc(viewerId)) continue;
        auto& player = clients[viewerId];
        {
            lock_guard<mutex> ll(player._cLock);
            if (s_state::ST_INGAME != player._state) continue;
        }
        if (player._id == id) continue;
        player.DoSendRemoveObjectPacket(id);
    }
    clients[id].Dispose();

    clients[id]._cLock.lock();
    clients[id]._state = s_state::ST_FREE;
    clients[id]._cLock.unlock();
}

void ProcessPacket(int cId, char* packet)
{
    switch (packet[1])
    {
        case CS_LOGIN:
        {
            OBDCManager obdcMgr;
            auto* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);

            auto exec = L"EXEC select_user_data "s;
            auto s = ""s;
            auto ws = L""s;

            s.append(p->name);
            ws.assign(s.begin(), s.end());
            wprintf(ws.c_str());

            exec.append(ws.c_str());
            obdcMgr.AllocateHandles();
            obdcMgr.ConnectDataSource(L"2022GAMESERVEROBDC");
            obdcMgr.ExecuteStatementDirect(exec.c_str());
            obdcMgr.RetrieveResult();

            if (obdcMgr.user_lv == -1)
            {
                SC_LOGIN_FAIL_PACKET fp;
                fp.size = sizeof(SC_LOGIN_FAIL_PACKET);
                fp.type = SC_LOGIN_FAIL;
                clients[cId].DoAsyncSend(&fp);
                break;
            }
            else
            {
                strcpy_s(clients[cId]._name, p->name);
                {
                    lock_guard<mutex> ll{ clients[cId]._cLock };
                    clients[cId]._x = obdcMgr.user_x;
                    clients[cId]._y = obdcMgr.user_y;
                    clients[cId]._hp = obdcMgr.user_hp;
                    clients[cId]._maxHp = obdcMgr.user_max_hp;
                    clients[cId]._level = obdcMgr.user_lv;
                    clients[cId]._exp = obdcMgr.user_exp;
                    clients[cId]._state = s_state::ST_INGAME;
                }

                clients[cId].DoSendAccountPacket();
                for (auto& client : clients)
                {
                    {
                        lock_guard<mutex> ll(client._cLock);
                        if (s_state::ST_INGAME != client._state) continue;
                    }
                    if (client._id == cId) continue;
                    if (false == IsVisible(cId, client._id)) continue;
                    if (IsPlayer(client._id)) client.DoSendAddObjectPacket(cId, clients);
                    clients[cId].DoSendAddObjectPacket(client._id, clients);
                }
            }
            obdcMgr.DisconnectDataSource();
            break;
        }
        case CS_MOVE:
        {
            auto* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
            clients[cId].lastMoveTime = p->move_time;
            auto curX = clients[cId]._x;
            auto curY = clients[cId]._y;
            switch (p->direction)
            {
                case 0: if (curY > 0) curY--; break;
                case 1: if (curY < constant::win_height - 1) curY++; break;
                case 2: if (curX > 0) curX--; break;
                case 3: if (curX < constant::win_width - 1) curX++; break;
            }

            clients[cId]._x = curX;
            clients[cId]._y = curY;
            // -----------방향이동-------------


            auto nearList = unordered_set<uint32_t>{};
            clients[cId]._vwLock.lock();
            auto oldVwList = clients[cId]._vwList;
            clients[cId]._vwLock.unlock();

            for (auto& client : clients)
            {
                if (client._state != s_state::ST_INGAME) continue;
                if (client._id == cId) continue;
                if (IsVisible(cId, client._id)) nearList.insert(client._id);
            }

            clients[cId].DoSendMovePacket(cId, clients);

            for (auto& viewerId : nearList)
            {
                auto& viewer = clients[viewerId];
                if (IsPlayer(viewerId))
                {
                    viewer._vwLock.lock();
                    if (clients[viewerId]._vwList.count(cId))
                    {
                        viewer._vwLock.unlock();
                        clients[viewerId].DoSendMovePacket(cId, clients);
                    }
                    else
                    {
                        viewer._vwLock.unlock();
                        clients[viewerId].DoSendAddObjectPacket(cId, clients);
                    }
                }
                if (oldVwList.count(viewerId) == 0) clients[cId].DoSendAddObjectPacket(viewerId, clients);
            } // 새로 리스트에 추가됐거나 전에 있던 사람이라면

            for (auto& viewerId : oldVwList)
            {// 전에 있던 리스트에서 사라진다면
                if (0 == nearList.count(viewerId))
                {
                    clients[cId].DoSendRemoveObjectPacket(viewerId);
                    if (IsPlayer(viewerId))
                    {
                        clients[viewerId].DoSendRemoveObjectPacket(cId);
                    }
                }
            }
            break;
        }
        case CS_ATTACK:
        {
            CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
            for (auto& i : clients[cId]._vwList)
            {
                if (IsDetected(cId, i) && i > 10000)
                {
                    clients[i]._hp -= 5;
                    std::cout << i << " NPC 데미지 " << std::endl;
                    if (clients[i]._hp < 0)
                    {
                        clients[cId]._exp += 10;
                        auto packet = SC_EXP_PACKET{};
                        packet.size = sizeof(SC_EXP_PACKET);
                        packet.type = SC_EXP;
                        packet.exp = 10;
                        clients[cId].DoAsyncSend(&packet);
                        clients[i]._hp = 100;
                    }
                }
            }
        }
    }
}




void worker_thread(HANDLE hIocp)
{
    while (true) 
    {
        DWORD nBytes(0);
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(hIocp, &nBytes, &key, &over, INFINITE);
        OverEX* ex_over = reinterpret_cast<OverEX*>(over);
        if (FALSE == ret)
        {
            if (ex_over->_cmpType == OP_ACCEPT) cout << "Accept Error";
            else
            {
                cout << "GQCS Error on client[" << key << "]\n";
                Disconnect(static_cast<int>(key));
                if (ex_over->_cmpType == OP_SEND) delete ex_over;
                continue;
            }
        }

        if ((0 == nBytes) && ((ex_over->_cmpType == OP_RECV) || (ex_over->_cmpType == OP_SEND)))
        {
            Disconnect(static_cast<int>(key));
            if (ex_over->_cmpType == OP_SEND) delete ex_over;
            continue;
        }

        switch (ex_over->_cmpType)
        {
        case OP_ACCEPT: 
        {
            auto clientId = GetNewClientId();
            if (clientId != -1) 
            {
                {
                    auto lg = lock_guard<mutex>{ clients[clientId]._cLock };
                    clients[clientId]._state = s_state::ST_ALLOC;
                    clients[clientId].Init(clientId, glClientSock);
                }
               
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(glClientSock), hIocp, clientId, 0);
                clients[clientId].DoAsyncRecv();
                glClientSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            }
            else
            {
                cout << "Max user exceeded.\n";
            }
            ::ZeroMemory(&glAcceptOver, sizeof(WSAOVERLAPPED));
            auto addr_size = sizeof(sockaddr_in);
            AcceptEx(glServerSock, glClientSock, glAcceptOver._sendBuf, 0,
                addr_size + constant::sockaddr_padding,
                addr_size + constant::sockaddr_padding,
                nullptr,
                &glAcceptOver);

            break;
        }
        case OP_RECV:
        {
            auto remainSize = nBytes + clients[key]._prevRemainSize;
            auto* p = ex_over->_sendBuf;
            while (remainSize > 0)
            {
                auto packet_size = (uint8_t)p[0];
                if (packet_size <= remainSize)
                {
                    ProcessPacket(static_cast<int32_t>(key), p);
                    p = p + packet_size;
                    remainSize = remainSize - packet_size;
                }
                else break;
            }
            clients[key]._prevRemainSize = remainSize;
            if (remainSize > 0) {
                memcpy(ex_over->_sendBuf, p, remainSize);
            }
            clients[key].DoAsyncRecv();
            break;
        }
        case OP_SEND:
            delete ex_over;
            break;
        }
    }
}


void InitializeNPC()
{
    cout << "NPC intialize begin.\n";
    for (int i = constant::max_user; i < constant::max_user + constant::max_npc; ++i) {
        clients[i]._x = rand() % constant::win_width + 500;
        clients[i]._y = rand() % constant::win_height;
        clients[i]._id = i;
        clients[i]._hp = 100;
        sprintf_s(clients[i]._name, "NPC%d", i);
        clients[i]._state = s_state::ST_INGAME;
       
    }
    cout << "NPC initialize end.\n";
}



int main()
{
    auto WSAData = WSADATA{};
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    glServerSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    
    auto serverAddr = sockaddr_in{};
    ::ZeroMemory(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(constant::port_num);
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(glServerSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    listen(glServerSock, SOMAXCONN);
    
    auto clientAddr = sockaddr_in{};
    auto addrLen = sizeof(clientAddr);
    InitializeNPC();

    hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(glServerSock), hIocp, 9999, 0);
    glClientSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    glAcceptOver._cmpType = OP_ACCEPT;
    AcceptEx(glServerSock, glClientSock,
        glAcceptOver._sendBuf,
        0, 
        addrLen + constant::sockaddr_padding,
        addrLen + constant::sockaddr_padding,
        0,
        &glAcceptOver);

    std::vector <std::jthread> workerThreads;
    int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; ++i)
    {
        workerThreads.emplace_back(worker_thread, hIocp);
    }
   
    closesocket(glServerSock);   
    WSACleanup();
}