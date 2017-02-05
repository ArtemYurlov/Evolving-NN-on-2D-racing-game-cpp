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
	bool Init (Game* game);
	bool LevelFromChechpointWidth(const vector<t_cpWidth> &cpWidths, const sf::Vector2f &leftPoint);
	bool isValid() const;

	void Draw() const;

	//get/set
	sf::Vector2f getSpawnPos() const;
	
	vector<t_line> getWalls();
	vector<t_line> getCPs();

	float getSpawnAng() const;

private:
	//void buildCheckpoints();
	
	Game* game;

	sf::Vector2f m_spawnPos;
	float m_spawnAng;

	vector<sf::Vector2f> m_wall_1; // a stream of points to be connected into the 1st wall
	vector<sf::Vector2f> m_wall_2; // 2nd wall
	vector<t_line> m_checkLines;

	bool m_valid;

};

