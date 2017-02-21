#pragma once
#include <SFML/Graphics.hpp>

#include "Car.h"
#include "Level.h"
#include  "NNet.h"
#include  "Replicator.h"
#include <memory>

using namespace std;

class Game
{
public:
	Game();
	virtual ~Game() {  }

	//Methods
	virtual bool Init( sf::RenderWindow* rWnd);
	virtual void EventHandle();
	virtual void Update(float dt);
	virtual void Render();
	virtual void Quit();

	//get/set
	void setLevel(const Level &level);
	Level getLevel() const;

	vector<t_line> getCPs() const;
	vector<t_line> getWalls() const;

	void addCar(Car* car);
	void setCars(const vector<Car*> &cars);
	vector<Car*> getCars() const;
	void clearCars();

	sf::RenderWindow* getRenderWindow() const;

	float getScoreForCompleation() const { return m_scoreForCompleation; }
	void setScoreForCompleation(const float &score) { m_scoreForCompleation = score; }


protected:
	Level m_level;
	vector<Car*> m_cars;
	vector<t_line> m_walls;
	vector<t_line> m_cps; // check points

	float m_scoreForCompleation;
	float m_sp_scoreForCompleation;

	sf::RenderWindow* m_rWnd;
	sf::Vector2f m_camPos;
};


class Evolution_Controller : public Game
{
public:
	Evolution_Controller();

	bool Init(sf::RenderWindow* rWnd) override;
	void Update(float dt) override;

	void Render() override;


private:
	
	unique_ptr<NNTopology> m_pNNTopology;
	vector<NNet> m_NNets;

	vector<Replicator*> m_replicators;
};