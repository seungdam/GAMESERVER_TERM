#pragma once
class CEntity
{
	static sf::Texture _graphics;
public:
	CEntity() = default;
	virtual void ProcessInput() = 0;
	virtual void Update() = 0;
	virtual void Render(sf::RenderWindow&) = 0;
	virtual ~CEntity() = default;
};

