#include "Level.h"

#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>
#include <iostream>

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

bool Level::Init(Game* game)
{
	this->game = game;

	/*m_wall_1.push_back(sf::Vector2f(0.f, 0.f));
	m_wall_1.push_back(sf::Vector2f(0.f, 200.f));
	m_wall_1.push_back(sf::Vector2f(-200.f, 200.f));

	m_wall_2.push_back(sf::Vector2f(-100.f, 0.f));
	m_wall_2.push_back(sf::Vector2f(-100.f, 100.f));
	m_wall_2.push_back(sf::Vector2f(-200.f, 100.f));

	vector<t_cpWidth> cpWs;
	cpWs.push_back(t_cpWidth(sf::Vector2f(0.f, 0.f), 70.f,1));// spawn line

	cpWs.push_back(t_cpWidth(sf::Vector2f(0.f, -500.f), 70.f,7));
	cpWs.push_back(t_cpWidth(sf::Vector2f(-50.f, -600.f), 50.f,4));
	cpWs.push_back(t_cpWidth(sf::Vector2f(-100.f, -700.f), 40.f, 5));
	cpWs.push_back(t_cpWidth(sf::Vector2f(0.f, -850.f), 50.f, 6));

	LevelFromChechpointWidth(cpWs, sf::Vector2f(0.f, -1.f));*/

	vector<t_angLenWidth> ang_len_widths;

	ang_len_widths.push_back(t_angLenWidth(0.f, 1200.f, 150.f, 11));

	ang_len_widths.push_back(t_angLenWidth(-45.f, 100.f, 100.f, 3));
	ang_len_widths.push_back(t_angLenWidth(-45.f, 400.f, 71.f, 15));

	ang_len_widths.push_back(t_angLenWidth(45.f, 50.f, 71.f, 5));
	ang_len_widths.push_back(t_angLenWidth(45.f, 500.f, 71.f * 0.64f, 15));

	ang_len_widths.push_back(t_angLenWidth(45.f, 50.f, 71.f * 0.64f, 5));
	ang_len_widths.push_back(t_angLenWidth(45.f, 400.f, 100.f, 5));

	ang_len_widths.push_back(t_angLenWidth(0.f, 20.f, 100.f, 1));

	ang_len_widths.push_back(t_angLenWidth(0.f, 400.f, 40.f, 5));

	ang_len_widths.push_back(t_angLenWidth(45.f, 50.f, 30.f, 5));
	ang_len_widths.push_back(t_angLenWidth(45.f, 50.f, 20.f, 5));

	ang_len_widths.push_back(t_angLenWidth(0.f, 400.f, 20.f, 20));



	LevelFromAngleLengthWidth(ang_len_widths, sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, -1.f));



	m_spawnPos = sf::Vector2f(0.f, -40.f);
	m_spawnAng = -90.f; //facing north

	m_valid = true;

	return true;
}

bool Level::LevelFromAngleLengthWidth(const vector<t_angLenWidth>& angLenWidths, const sf::Vector2f &firstPoint, const sf::Vector2f& initDir)
{
	vector<t_cpWidth> cpWidths;
	cpWidths.push_back(t_cpWidth(firstPoint, angLenWidths[0].w, 1)); //initial point

	sf::Vector2f dir; // direction of the current iteration
	sf::Vector2f prevDir = initDir; // direction of the previous iteration
	for (unsigned i = 0; i < angLenWidths.size(); ++i)
	{
		dir = Normalize( RotateVec2f(prevDir, sf::Vector2f(0.f,0.f), angLenWidths[i].a));
		cpWidths.push_back(t_cpWidth(cpWidths.back().cp + dir * angLenWidths[i].l
			, angLenWidths[i].w, angLenWidths[i].numCp));
		prevDir = dir;
	}

	return LevelFromChechpointWidth(cpWidths, initDir);
}


bool Level::LevelFromChechpointWidth(const vector<t_cpWidth> &cpWidths, const sf::Vector2f &initDir)
{
	sf::Vector2f dir = Normalize(initDir);
	m_checkLines.push_back(t_line(cpWidths[0].cp - Perp(dir)*cpWidths[0].width, cpWidths[0].cp + Perp(dir)*cpWidths[0].width));
	for (unsigned i = 1; i < cpWidths.size(); ++i)
	{
		dir = Normalize(cpWidths[i].cp - cpWidths[i - 1].cp);
		t_line finalLine = t_line(cpWidths[i].cp - Perp(dir)*cpWidths[i].width, cpWidths[i].cp + Perp(dir)*cpWidths[i].width);
		t_line initLine = m_checkLines.back();

		if (cpWidths[i].numCp == 0)
			continue;

		for (unsigned j = 1; j <= cpWidths[i].numCp; j++)
		{
			float frac = float(j) / float(cpWidths[i].numCp);
			t_line line = t_line(initLine.p1 + (frac * (finalLine.p1 - initLine.p1)), initLine.p2 + (frac * (finalLine.p2 - initLine.p2)));
			m_checkLines.push_back(line);
		}
	}

	// fill in the walls
	for (auto &l : m_checkLines)
	{
		m_wall_1.push_back(l.p1);
		m_wall_2.push_back(l.p2);
	}

	return true;
}


bool Level::isValid() const
{
	return m_valid;
}
void Level::Draw() const
{
	glLineWidth(3.f);

	//draw checkLines
	for (auto &l : m_checkLines)
		drawLine(game->getRenderWindow(), l, sf::Color(112, 112, 112), sf::Color(112, 112, 112));

	//draw walls
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

	if (m_wall_1.size() == 0)
		return walls;
	if (m_wall_2.size() == 0)
		return walls;

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

	//last wall
	/*wall.p1 = m_wall_1[m_wall_1.size() - 1];
	wall.p2 = m_wall_2[m_wall_2.size() - 1];*/
	walls.push_back(wall);

	return walls;
}

vector<t_line> Level::getCPs()
{
	return m_checkLines;
}

float Level::getSpawnAng() const
{
	return m_spawnAng;
}
