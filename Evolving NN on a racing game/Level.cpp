#include "Level.h"

#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>

Level::Level()
{
	game = nullptr;
	m_spawnPos = sf::Vector2f(0.f, 0.f);
	m_spawnAng = 0.f;

	m_valid = false;
}


Level::~Level()
{
}

bool Level::LoadLevel(Game* game)
{
	this->game = game;

	m_wall_1.push_back(sf::Vector2f(0.f, 0.f));
	m_wall_1.push_back(sf::Vector2f(0.f, 200.f));
	m_wall_1.push_back(sf::Vector2f(-200.f, 200.f));

	m_wall_2.push_back(sf::Vector2f(-100.f, 0.f));
	m_wall_2.push_back(sf::Vector2f(-100.f, 100.f));
	m_wall_2.push_back(sf::Vector2f(-200.f, 100.f));

	m_spawnPos = sf::Vector2f(-50.f, 50.f);
	m_spawnAng = 90.f; //facing south

	m_valid = true;

	return true;
}
bool Level::isValid() const
{
	return m_valid;
}
void Level::Draw() const
{
	glLineWidth(3.f);

	drawConnected(game->getRenderWindow(), m_wall_1);
	drawConnected(game->getRenderWindow(), m_wall_2);
	drawLine(game->getRenderWindow(), m_wall_1[0], m_wall_2[0]);
	drawLine(game->getRenderWindow(), m_wall_1.back(), m_wall_2.back(), sf::Color::Green, sf::Color::Green);


	drawPoint(game->getRenderWindow(), m_spawnPos, 7.f, sf::Color::Green);

	glLineWidth(1.f);
}

//get/set
sf::Vector2f Level::getSpawnPos() const
{
	return m_spawnPos;
}

vector<t_line> Level::getWalls()
{
	vector<t_line> walls;
	for (unsigned i = 0; i < m_wall_1.size() - 1; ++i)
	{
		t_line wall;
		wall.p1 = m_wall_1[i];
		wall.p2 = m_wall_1[i + 1];
		walls.push_back(wall);
	}
	for (unsigned i = 0; i < m_wall_2.size() - 1; ++i)
	{
		t_line wall;
		wall.p1 = m_wall_2[i];
		wall.p2 = m_wall_2[i + 1];
		walls.push_back(wall);
	}

	t_line wall;// fisrt wall
	wall.p1 = m_wall_1[0];
	wall.p2 = m_wall_2[0];
	walls.push_back(wall);

	wall.p1 = m_wall_1[m_wall_1.size() - 1];
	wall.p2 = m_wall_2[m_wall_2.size() - 1];
	walls.push_back(wall);

	return walls;
}

float Level::getSpawnAng() const
{
	return m_spawnAng;
}
