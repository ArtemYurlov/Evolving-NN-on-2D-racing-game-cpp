#pragma once
#include <SFML/Graphics.hpp>

#include "Car.h"
#include "Level.h"

using namespace std;

class Game
{
public:
	Game();
	~Game();

	//Methods
	bool Init( sf::RenderWindow* rWnd);
	void EventHandle();
	void Update(float dt);
	void Render();
	void Quit();

	//get/set
	void setLevel(const Level &level);
	Level getLevel() const;

	vector<t_line> getCPs() const;
	vector<t_line> getWalls() const;

	void addCar(Car* car);
	void setCars(const vector<Car*> &cars);
	vector<Car*> getCars() const;
	void clearCars();

	sf::RenderWindow* getRenderWindow() const;



private:
	Level m_level;
	vector<Car*> m_cars;
	vector<t_line> m_walls;
	vector<t_line> m_cps; // check points

	sf::RenderWindow* m_rWnd;
	sf::Vector2f m_camPos;
};

