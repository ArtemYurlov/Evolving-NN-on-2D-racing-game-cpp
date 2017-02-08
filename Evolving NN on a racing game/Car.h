#pragma once

#include <SFML/Graphics.hpp>
#include "Utils.h"
#include <array>

class Game;
class NNet;

class Car
{
public:
	Car();
	virtual ~Car() = default;

	// Methods
	virtual bool Init(Game* game, const sf::Vector2f &spawnPos, const float &spawnAng);
	virtual void EventHandle() = 0;
	virtual void Update(float dt);
	virtual void Draw() const;

	void Kill();
	bool Revive();

	//get/set
	sf::Vector2f getPos() const;
	sf::Vector2f getDirectionFacing() const;
	vector<t_line> getEdges() const;
	vector<sf::Vector2f> getVertices() const;

	bool isAlive() const;
	bool isPlayer() const;
	virtual void addScore(); //increase score
	virtual float getScore() const ; 
	void addRun();
	void setRuns(const unsigned &runs);
	unsigned getRuns() const;
	

protected:
	void EventClear(); // clear the events after the eventHandle
	void updateCarBox(); // update the position of the cars box based on the cars position

	float mm_h; //size of the car
	float mm_w;

	sf::Vector2f m_sp_pos; //spawn position
	float m_sp_ang;

	Game* game; //pointer to the game instance
	sf::Vector2f m_pos; // current pos
	float m_ang; // angle facing eg 0 = east increasing cloackwise
	sf::Vector2f m_vel; // current vel
	float m_speed;
	float m_maxAcc; //maximum acceleration of the car
	sf::Vector2f m_acc; //acceleration vector
	float m_fric; //friction, the coefficient of speed the car will slowdown with every tick
	float m_turnCoef; //the sharpnes of a turn coeff

	int m_bAcc; //is car accelerating [1 - forward 0 - no -1 - back]
	int m_turn; //is car turning [-1 - left 0 - no 1 - right]

	bool m_isPlayer;

	unsigned m_runs; //number of runs the car has after its death
	float m_score; //score of a car

	vector<sf::Vector2f> m_trail;
	vector<t_line> m_checkPointsLeft; //checkpoints left to score for the car

	bool m_alive; // is the car alive
	float m_reviveIn; // when to revive after the crash, set negative to disable
	float m_timeDead; // local variable to keep track of time dead

	sf::Color m_color; //color with witch the car would be drawn
	vector <sf::Vector2f> m_vertices; //4 corners of the car define its box

protected:
	vector<t_line> m_l_walls;

	bool m_m_wasDead;
};

class PlayerCar:public Car
{
public:
	PlayerCar();
	
	void EventHandle() override;
	
private:

};

class AICar : public Car
{
public:
	struct t_sensor
	{
		t_sensor() { this->line = t_line(); value = 0.f; }
		t_sensor(const t_line line, const float value) { this->line = line; this->value = value; }
		t_line line;
		float value;
	};

public:
	AICar();
	~AICar();

	bool Init(Game* game, const sf::Vector2f &spawnPos, const float &spawnAng) override;
	void EventHandle() override;
	void Update(float dt) override;
	void Draw() const override;

	void addScore() override;
	float getScore() const override;

	NNet* getNNetPtr() const;


private:
	void updateSensors();



	float m_timeAlive;
	float m_timeSinceCp;

	float m_o_turn;
	float m_o_acc;

	NNet* m_NN;

	array<t_sensor, 5> m_sensors;

};
