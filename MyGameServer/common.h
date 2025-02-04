#pragma once
// IO
#include <iostream>

// Winsock2
#include <WS2tcpip.h>
#include <MSWSock.h>

// STL CONTAINER
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <array>
#include <queue>

// Concurrency
#include <thread>
#include <concurrent_priority_queue.h>
#include <mutex>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

struct constant
{
	static constexpr int32_t port_num	= 4000;
	static constexpr int32_t buf_len	= 200;
	static constexpr int32_t name_len	= 20;
	static constexpr int32_t chat_len	= 100;
	static constexpr int32_t max_user	= 10000;
	static constexpr int32_t max_npc	= 20000;
	static constexpr int32_t win_width	= 2000;
	static constexpr int32_t win_height	= 2000;
	static constexpr int16_t sockaddr_padding = 16;
	static constexpr int32_t view_range = 5;
};

enum class s_state : uint8_t { ST_FREE, ST_ALLOC, ST_INGAME };

#include "../protocol/protocol.h"