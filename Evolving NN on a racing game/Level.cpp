#include "Level.h"



Level::Level()
{
}


Level::~Level()
{
}

bool Level::LoadLevel(const Game* game)
{
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
