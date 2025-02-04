#pragma once
class CGameManager;

struct GameManagerDeleter
{
	void operator()(CGameManager*) {};
};

class CGameManager : public CSingleton<CGameManager, GameManagerDeleter>
{
public:
	CGameManager() { Init(); };
	void Init() override;
	void ResetGame() {};
	void Update();
	void Render();
	~CGameManager() = default;
private:
	enum class GameState { Paused, Running };
	sf::RenderWindow _winGame{ {constants::window_width, constants::window_height },"Game Client 1.0" };
	sf::Event event;
};

