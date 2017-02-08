#pragma once
#include <SFML/Graphics.hpp>

#include "Car.h"
#include "Level.h"
#include  "NNet.h"
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



protected:
	Level m_level;
	vector<Car*> m_cars;
	vector<t_line> m_walls;
	vector<t_line> m_cps; // check points

	sf::RenderWindow* m_rWnd;
	sf::Vector2f m_camPos;
};


class Evolution_Controller : public Game
{
public:
	struct t_genome
	{
		unsigned id;
		unsigned index;
		float fitness;
		vector<float> weights;
	};

	Evolution_Controller();
	bool Init(sf::RenderWindow* rWnd) override;
	void Update(float dt) override;

	/* OLD 
	void GenerateRandPopulation();

	void BreedPopulation();
	void CrossBreed(const t_genome &g1, const t_genome &g2, t_genome &baby1, t_genome &baby2);

	void RateGenome(const unsigned &index, const float &score); */



private:
	/*vector<t_genome> getBestGenomes(const unsigned num);
	static void mutate(t_genome &gen);

	unsigned m_curr_id;
	unsigned m_generation;
	float m_best_fitness;
	t_genome m_alpha_genome;

	vector<t_genome> m_genomes;*/

	unique_ptr<NNTopology> m_pNNTopology;
	vector<NNet> m_NNets;

};