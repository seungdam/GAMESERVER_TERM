#include "Msession.h"


#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

constexpr int VIEW_RANGE = 5;



HANDLE h_iocp;
array<Msession, MAX_USER + MAX_NPC> clients;

SOCKET g_s_socket, g_c_socket;
OVER_EXP g_a_over;

bool is_pc(int object_id)
{
    return object_id < MAX_USER;
}

bool is_npc(int object_id)
{
    return !is_pc(object_id);
}

bool can_see(int from, int to)
{
    if (abs(clients[from].x - clients[to].x) > VIEW_RANGE) return false;
    return abs(clients[from].y - clients[to].y) <= VIEW_RANGE;
}

bool can_dmg(int from, int to) {
    if (abs(clients[from].x - clients[to].x) > 1) return false;
    return abs(clients[from].y - clients[to].y) <= 1;
}


int get_new_client_id()
{
    for (int i = 0; i < MAX_USER; ++i) {
        lock_guard <mutex> ll{ clients[i]._s_lock };
        if (clients[i]._state == ST_FREE)
            return i;
    }
    return -1;
}

void process_packet(int c_id, char* packet)
{
    switch (packet[1]) {
    case CS_LOGIN: {
        CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
        strcpy_s(clients[c_id]._name, p->name);
        {
            lock_guard<mutex> ll{ clients[c_id]._s_lock };
            clients[c_id].x = rand() % W_WIDTH;
            clients[c_id].y = rand() % W_HEIGHT;
            clients[c_id]._state = ST_INGAME;
        }
        clients[c_id].send_login_info_packet();
        for (auto& pl : clients) {
            {
                lock_guard<mutex> ll(pl._s_lock);
                if (ST_INGAME != pl._state) continue;
            }
            if (pl._id == c_id) continue;
            if (false == can_see(c_id, pl._id))
                continue;
            if (is_pc(pl._id)) pl.send_add_object_packet(c_id,clients);
            clients[c_id].send_add_object_packet(pl._id,clients);
        }
        break;
    }
    case CS_MOVE: {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        clients[c_id].last_move_time = p->move_time;
        short x = clients[c_id].x;
        short y = clients[c_id].y;
        switch (p->direction) {
        case 0: if (y > 0) y--; break;
        case 1: if (y < W_HEIGHT - 1) y++; break;
        case 2: if (x > 0) x--; break;
        case 3: if (x < W_WIDTH - 1) x++; break;
        }
       
        clients[c_id].x = x;
        clients[c_id].y = y;
        // -----------방향이동-------------


        unordered_set<int> near_list;
        clients[c_id]._vl.lock();
        unordered_set<int> old_vlist = clients[c_id]._view_list;
        clients[c_id]._vl.unlock();
        for (auto& cl : clients) {
            if (cl._state != ST_INGAME) continue;
            if (cl._id == c_id) continue;
            if (can_see(c_id, cl._id))
                near_list.insert(cl._id);
        }

        clients[c_id].send_move_packet(c_id,clients);

        for (auto& pl : near_list) {
            auto& cpl = clients[pl];
            if (is_pc(pl)) {
                cpl._vl.lock();
                if (clients[pl]._view_list.count(c_id)) {
                    cpl._vl.unlock();
                    clients[pl].send_move_packet(c_id,clients);
                }
                else {
                    cpl._vl.unlock();
                    clients[pl].send_add_object_packet(c_id,clients);
                }
            }
 
            if (old_vlist.count(pl) == 0)
                clients[c_id].send_add_object_packet(pl,clients);
        } // 새로 리스트에 추가됐거나 전에 있던 사람이라면

        for (auto& pl : old_vlist) // 전에 있던 리스트에서 사라진다면
            if (0 == near_list.count(pl)) {
                clients[c_id].send_remove_object_packet(pl);
                if (is_pc(pl))
                    clients[pl].send_remove_object_packet(c_id);
            }
    }
           break;
    case CS_ATTACK: {
        CS_ATTACK_PACKET* p = reinterpret_cast<CS_ATTACK_PACKET*>(packet);
        for (auto& i : clients[c_id]._view_list) {
            if (can_dmg(c_id,i) && i < 10000) {
                clients[i]._hp -= 5;
                std::cout << i << " 번 클라 데미지!" << std::endl;
                if (clients[i]._hp < 0) {
                    clients[c_id]._exp += 10;
                }
            }
        }
    }
     
    }
}

void disconnect(int c_id)
{
    clients[c_id]._vl.lock();
    unordered_set <int> vl = clients[c_id]._view_list;
    clients[c_id]._vl.unlock();
    for (auto& p_id : vl) {
        if (is_npc(p_id)) continue;
        auto& pl = clients[p_id];
        {
            lock_guard<mutex> ll(pl._s_lock);
            if (ST_INGAME != pl._state) continue;
        }
        if (pl._id == c_id) continue;
        pl.send_remove_object_packet(c_id);
    }
    closesocket(clients[c_id]._socket);

    lock_guard<mutex> ll(clients[c_id]._s_lock);
    clients[c_id]._state = ST_FREE;
}


void worker_thread(HANDLE h_iocp)
{
    while (true) {
        DWORD num_bytes(0);
        ULONG_PTR key;
        WSAOVERLAPPED* over = nullptr;
        BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
        OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
        if (FALSE == ret) {
            if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
            else {
                cout << "GQCS Error on client[" << key << "]\n";
                disconnect(static_cast<int>(key));
                if (ex_over->_comp_type == OP_SEND) delete ex_over;
                continue;
            }
        }

        if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
            disconnect(static_cast<int>(key));
            if (ex_over->_comp_type == OP_SEND) delete ex_over;
            continue;
        }

        switch (ex_over->_comp_type) {
        case OP_ACCEPT: {
            int client_id = get_new_client_id();
            if (client_id != -1) {
                {
                    lock_guard<mutex> ll(clients[client_id]._s_lock);
                    clients[client_id]._state = ST_ALLOC;
                }
                clients[client_id].x = 0;
                clients[client_id].y = 0;
                clients[client_id]._id = client_id;
                clients[client_id]._name[0] = 0;
                clients[client_id]._prev_remain = 0;
                clients[client_id]._socket = g_c_socket;
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_c_socket),
                    h_iocp, client_id, 0);
                clients[client_id].do_recv();
                g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            }
            else {
                cout << "Max user exceeded.\n";
            }
            ZeroMemory(&g_a_over._over, sizeof(g_a_over._over));
            int addr_size = sizeof(SOCKADDR_IN);
            AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

            break;
        }
        case OP_RECV: {
            int remain_data = num_bytes + clients[key]._prev_remain;
            char* p = ex_over->_send_buf;
            while (remain_data > 0) {
                int packet_size = p[0];
                if (packet_size <= remain_data) {
                    process_packet(static_cast<int>(key), p);
                    p = p + packet_size;
                    remain_data = remain_data - packet_size;
                }
                else break;
            }
            clients[key]._prev_remain = remain_data;
            if (remain_data > 0) {
                memcpy(ex_over->_send_buf, p, remain_data);
            }
            clients[key].do_recv();
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
    for (int i = MAX_USER; i < MAX_USER + MAX_NPC; ++i) {
        clients[i].x = rand() % W_WIDTH;
        clients[i].y = rand() % W_HEIGHT;
        clients[i]._id = i;
        sprintf_s(clients[i]._name, "NPC%d", i);
        clients[i]._state = ST_INGAME;
       
    }
    cout << "NPC initialize end.\n";
}



int main()
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    g_s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUM);
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(g_s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    listen(g_s_socket, SOMAXCONN);
    SOCKADDR_IN cl_addr;
    int addr_size = sizeof(cl_addr);

    InitializeNPC();

    h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_s_socket), h_iocp, 9999, 0);
    g_c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    g_a_over._comp_type = OP_ACCEPT;
    AcceptEx(g_s_socket, g_c_socket, g_a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &g_a_over._over);

    vector <thread> worker_threads;
    int num_threads = std::thread::hardware_concurrency();
    for (int i = 0; i < num_threads; ++i)
        worker_threads.emplace_back(worker_thread, h_iocp);
  
    for (auto& th : worker_threads)
        th.join();
    closesocket(g_s_socket);
    WSACleanup();
}