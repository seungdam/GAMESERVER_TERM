#pragma once


enum class KEY_TYPE : short
{
	MLBUTTON = 0x01,
	SPACE = 0x20,

	UP = 0x26,
	RIGHT = 0x27,
	DOWN = 0x28,
	LEFT = 0x29,
	
	W = 0x57,
	A = 0x41,
	S = 0x53,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,

	NUM0 = 0x30,
	NUM1 = 0x31,
	NUM2 = 0x32,
	NUM3 = 0x33,
	NUM4 = 0x34,
	NUM5 = 0x35,
	NUM6 = 0x36,
	NUM7 = 0x37,
	NUM8 = 0x38,
	NUM9 = 0x39,
};
enum class KEY_STATE : uint8 { KEY_NONE = 0, KEY_DOWN, KEY_PRESSED, KEY_UP };

class CInputManager;

struct InputManagerDeleter
{
	void operator()(CInputManager*) {}
};

class CInputManager : public CSingleton<CInputManager,InputManagerDeleter>
{
public:
	CInputManager() { for (auto& s : _keyBuffer) s = KEY_STATE::KEY_NONE; }
	~CInputManager() {}
	void Init() override;
	void Update() override;
	void SetKeyInputState(const KEY_TYPE& key, const KEY_STATE& key_state);
	KEY_STATE GetKeyInputState(const KEY_TYPE& key);
private:
	KEY_STATE _keyBuffer[constants::key_count] {};
};

