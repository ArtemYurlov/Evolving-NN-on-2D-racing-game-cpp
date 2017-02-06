#pragma once
#include <SFML/Graphics.hpp>

#include "Utils.h"

using namespace std;

class Game;





class Level
{
public:
	struct t_cpWidth //struct of a checkpoint with width of the track around it
	{
		t_cpWidth() { this->cp = sf::Vector2f(0.f, 0.f); this->width = 0.f; this->numCp = 0; };
		t_cpWidth(const sf::Vector2f &cp, const float width, const unsigned numCp) { this->cp = cp; this->width = width; this->numCp = numCp; }
		sf::Vector2f cp;
		float width;
		unsigned numCp;
	};

	struct t_angLenWidth
	{
		t_angLenWidth(const float angle, const float length, const float width, const unsigned numCp)
		{ a = angle;l = length;w = width; this->numCp = numCp; }

		float a;
		float l;
		float w;
		unsigned numCp;
	};

public:
	Level();
	~Level();

	//Methods
	bool Init (Game* game);
	bool LevelFromChechpointWidth(const vector<t_cpWidth> &cpWidths, const sf::Vector2f &initDir);
	bool LevelFromAngleLengthWidth(const vector<t_angLenWidth> &angLenWidth, const sf::Vector2f &firstPoint, const sf::Vector2f &initDir);
	// generates a level from a steam of relative angle of the track, length of it and width of it
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

