#include "Level.h"



Level::Level()
{
	m_spawnPos = sf::Vector2f(0.f, 0.f);
	m_spawnAng = 0.f;

	m_valid = false;
}


Level::~Level()
{
}

bool Level::LoadLevel(const Game* game)
{
	m_wall_1.push_back(sf::Vector2f(0.f, 0.f));
	m_wall_1.push_back(sf::Vector2f(0.f, 20.f));
	m_wall_1.push_back(sf::Vector2f(-20.f, 20.f));

	m_wall_2.push_back(sf::Vector2f(-10.f, 0.f));
	m_wall_2.push_back(sf::Vector2f(-10.f, 10.f));
	m_wall_2.push_back(sf::Vector2f(-20.f, 10.f));

	m_spawnPos = sf::Vector2f(200.f, 200.f);
	m_spawnAng = 0.f; //facing north

	m_valid = true;

	return true;
}
void Level::Draw()
{
	
}

//get/set
sf::Vector2f Level::getSpawnPos()
{
	return m_spawnPos;
}

float Level::getSpawnAng()
{
	return m_spawnAng;
}
