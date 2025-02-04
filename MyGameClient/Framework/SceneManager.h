#pragma once

class CSceneManager;

struct SceneManagerDeleter
{
	void operator()(CSceneManager*) {}
};

enum class SceneState { Title, Game, Menu };
class CScene;

class CSceneManager : public CSingleton<CSceneManager, SceneManagerDeleter>
{
public:
	CSceneManager() {};
	~CSceneManager() {};
	void Init() override;
	void Update() override;
private:
	enum class SceneState :  uint8_t {Title, Main, Pause};
	std::stack<SceneState> _targetScene;
	std::unordered_map<SceneState, CScene*> _scenes;
};


class CScene
{
};
