#include "Game.h"

Game::Game()
{
	m_rWnd = nullptr;
}
Game::~Game(){}

bool Game::Init(sf::RenderWindow* rWnd )
{
	m_rWnd = rWnd;

	//Load a level
	Level level;
	if (!level.LoadLevel(this))
		return false;

	m_level = level;
	
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
		car->Update(dt);
}

void Game::Render()
{
	m_level.Draw();
	for (auto &car : m_cars)
		car->Draw();
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


