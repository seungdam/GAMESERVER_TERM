

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;		
constexpr char CS_TELEPORT = 4;		
constexpr char CS_LOGOUT = 5;		

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;

constexpr char SC_EXP = 10;
constexpr char SC_HP = 11;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET 
{
	uint8_t size;
	char	type;
	char	name[constant::name_len];
};

struct CS_ATTACK_PACKET 
{
	uint8_t size;
	char type;
};

struct CS_MOVE_PACKET 
{
	uint8_t	size;
	char			type;
	uint8_t			direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned		move_time;
};

struct CS_CHAT_PACKET
{
	uint8_t size;
	char	type;
	char	msg[constant::chat_len];
};

struct CS_TELEPORT_PACKET
{
	uint8_t size;
	char	type;
};

struct CS_LOGOUT_PACKET
{
	uint8_t size;
	char	type;
};

struct SC_LOGIN_INFO_PACKET
{
	uint8_t		size;
	char		type;
	int32_t		id;
	int32_t		hp;
	int32_t		max_hp;
	int32_t		exp;
	int32_t		level;
	char		name[constant::name_len];
	int16_t		x, y;
};

struct SC_ADD_OBJECT_PACKET
{
	uint8_t size;
	char	type;
	int16_t	x, y;
	int32_t		id;
	char	name[constant::name_len];
};

struct SC_REMOVE_OBJECT_PACKET
{
	uint8_t size;
	char	type;
	int32_t	id;
};

struct SC_MOVE_OBJECT_PACKET
{
	uint8_t		size;
	char		type;
	int32_t		id;
	int16_t		x, y;
	uint32_t	move_time;
};

struct SC_CHAT_PACKET
{
	uint8_t		size;
	char		type;
	int32_t		id;
	char		msg[constant::chat_len];
};

struct SC_LOGIN_OK_PACKET
{
	uint8_t	size;
	char	type;
};

struct SC_LOGIN_FAIL_PACKET
{
	uint8_t size;
	char	type;
};

struct SC_STAT_CHANGE_PACKET
{
	uint8_t		size;
	char		type;
	int32_t		hp;
	int32_t		max_hp;
	int32_t		exp;
	int32_t		level;
};

struct SC_EXP_PACKET
{
	unsigned char size;
	char	type;
	int		exp;
};

struct SC_ATTACED_PACKET
{
	uint32_t	id;
	uint32_t	dmg;
};

#pragma pack (pop)