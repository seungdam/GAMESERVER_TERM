#pragma once
class CEntity abstract
{
	
public:
	virtual void ProcessInput() = 0;
	virtual void Update() = 0;
	virtual void Render(sf::RenderWindow&) = 0;
	virtual ~CEntity() {};
};

