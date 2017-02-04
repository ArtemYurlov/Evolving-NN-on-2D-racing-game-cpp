#pragma once
#include <SFML/Graphics.hpp>

class Game;

class Level
{
public:
	Level();
	~Level();

	//Methods
	bool LoadLevel(const Game* game);
	void Draw();

	//get/set
	sf::Vector2f getSpawnPos();
	
	float getSpawnAng();

private:
	sf::Vector2f m_spawnPos;
	float m_spawnAng;
};

