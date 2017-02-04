#pragma once

#include <SFML/Graphics.hpp>

class Game;

class Car
{
public:
	Car();
	virtual ~Car() = default;

	// Methods
	bool Init(Game* game, const sf::Vector2f &spawnPos, const float &spawnAng);
	virtual void EventHandle() = 0;
	virtual void Update(float dt);
	void Draw();

	void Kill();

	//get/set
	sf::Vector2f GetDirectionFacing() const;
	

protected:
	void EventClear();

	

	Game* game;
	sf::Vector2f m_pos;
	float m_ang;
	sf::Vector2f m_vel;
	float m_speed;
	float m_maxAcc;
	float m_fric; //friction, the coefficient of speed the car will slowdown with every tick
	float m_turnCoef;

	int m_acc; //is car accelerating [1 - forward 0 - no -1 - back]
	int m_turn; //is car turning [-1 - left 0 - no 1 - right]

	bool m_alive;

	sf::Color m_color; //color with witch the car would be drawn
};

class PlayerCar:public Car
{
public:
	PlayerCar();
	
	void EventHandle() override;
	
private:

};

