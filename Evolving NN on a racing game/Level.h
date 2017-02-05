#pragma once
#include <SFML/Graphics.hpp>

#include "Utils.h"

using namespace std;

class Game;


class Level
{
public:
	Level();
	~Level();

	//Methods
	bool LoadLevel(Game* game);
	bool isValid() const;

	void Draw() const;

	//get/set
	sf::Vector2f getSpawnPos() const;
	
	vector<t_line> getWalls();

	float getSpawnAng() const;

private:
	//void buildCheckpoints();
	
	Game* game;

	sf::Vector2f m_spawnPos;
	float m_spawnAng;

	vector<sf::Vector2f> m_wall_1; // a stream of points to be connected into the 1st wall
	vector<sf::Vector2f> m_wall_2; // 2nd wall
	vector<t_line> checkLines;

	bool m_valid;

};

