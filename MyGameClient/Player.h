#pragma once
class CPlayer : public CEntity
{
public:
	CPlayer() = default;
	~CPlayer() = default;

	void ProcessInput() override;
	void Update() override;
	void Render(sf::RenderWindow& win) override;
private:
	sf::Sprite _img;
	CCamera _camera;
	sf::Vector2f _dir;
	sf::Vector2f _velocity;
	sf::Vector2f _accelarity;
	sf::Vector2f _position;
};

