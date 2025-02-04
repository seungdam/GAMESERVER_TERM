#pragma once

class CSceneManager;

struct SceneManagerDeleter
{
	void operator()(CSceneManager*) {}
};

enum class GameScene { Title, Game, Menu };

class CSceneManager : public CSingleton<CSceneManager, SceneManagerDeleter>
{
public:
	CSceneManager() {};
	~CSceneManager() {};
	void Init() override;
	void Update() override;
private:
	enum class GameScene {Title, Main, Pause};
	std::stack<GameScene> _scenes;
};

