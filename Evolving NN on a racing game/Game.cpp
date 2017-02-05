#include "Game.h"
#include <iostream>

Game::Game()
{
	m_rWnd = nullptr;
}
Game::~Game(){}

bool Game::Init(sf::RenderWindow* rWnd )
{
	//setup view
	m_rWnd = rWnd;
	m_camPos = sf::Vector2f(0.f, 0.f);

	//Load a level
	Level level;
	if (!level.LoadLevel(this))
		return false;

	m_level = level;
	m_walls = m_level.getWalls(); //for future collision detection
	
	//Create a car
	Car* pCar = new PlayerCar;
	if (!pCar->Init(this, level.getSpawnPos(), level.getSpawnAng()))
		return false;

	m_cars.push_back(pCar);

	return true;
}

void Game::EventHandle()
{
	for (auto &car : m_cars)
		car->EventHandle();
}

void Game::Update(float dt)
{
	for (auto &car : m_cars)
	{
		// collision update
		vector<t_line> edges = car->getEdges();
		for (auto &wall : m_walls)
			for (auto &edge : edges)
			{
				if (CollisionSeqSeq(edge, wall) != sf::Vector2f(0.f, 0.f))
					car->Kill();
			}

		// update car
		car->Update(dt);
	}
}

void Game::Render()
{
	// level
	m_level.Draw();

	// cars
	for (auto &car : m_cars)
		car->Draw();

	// camera
	sf::View view;
	view.reset(sf::FloatRect(0, 0, 940, 1377));

	sf::Vector2f camTarget;

	unsigned i = 0;
	for (; i < m_cars.size(); ++i)
		if (m_cars[i]->isAlive())
			break;

	if (i < m_cars.size())
		camTarget = m_cars[i]->getPos();

	view.setCenter(camTarget);
	m_rWnd->setView(view);

}

void Game::Quit()
{
	for (auto &car : m_cars) {
		car->Kill();
		delete car;
	}
}

void Game::setLevel(const Level& level)
{
	m_level = level;
}

Level Game::getLevel() const
{
	return m_level;
}

void Game::addCar(Car* car)
{
	m_cars.push_back(car);
}

void Game::setCars(const vector<Car*> &cars)
{
	m_cars = cars;
}

vector<Car*> Game::getCars() const
{
	return m_cars;
}

void Game::clearCars()
{
	for (auto &car : m_cars) {
		car->Kill();
		delete car;
	}
	m_cars.clear();
}

sf::RenderWindow* Game::getRenderWindow() const
{
	return m_rWnd;
}


