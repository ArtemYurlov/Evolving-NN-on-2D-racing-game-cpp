#include "Game.h"
#include <iostream>
#include "NNet.h"

Game::Game()
{
	m_rWnd = nullptr;
}

bool Game::Init(sf::RenderWindow* rWnd )
{
	//setup view
	m_rWnd = rWnd;
	m_camPos = sf::Vector2f(0.f, 0.f);

	//Load a level
	Level level;
	if (!level.Init(this))
		return false;

	m_level = level;
	m_walls = m_level.getWalls(); //for future collision detection
	m_cps = m_level.getCPs();
	
	//Create a car
	Car* pCar = new PlayerCar;
	if (!pCar->Init(this, level.getSpawnPos(), level.getSpawnAng()))
		return false;

	m_cars.push_back(pCar);
	
	return true;
}

void Game::EventHandle()
{

}

void Game::Update(float dt)
{
	for (auto &car : m_cars)
	{		
		// update car
		car->Update(dt);

		// collision update
		vector<t_line> edges = car->getEdges();

		for (auto &wall : m_walls)
			for (auto &edge : edges)
			{
				if (CollisionSeqSeq(edge, wall) != sf::Vector2f(0.f, 0.f))
					car->Kill();
			}

	}
}

void Game::Render()
{
	// level
	m_level.Draw();

	// cars
	for (auto &car : m_cars)
		car->Draw();

	// camera
	sf::View view;
	view.reset(sf::FloatRect(0, 0, 940, 1377));

	sf::Vector2f camTarget;

	unsigned i = 0;
	for (; i < m_cars.size(); ++i)
		if (m_cars[i]->isAlive() || m_cars[i]->isPlayer())
			break;

	if (i < m_cars.size())
		camTarget = m_cars[i]->getPos();

	view.setCenter(camTarget);
	m_rWnd->setView(view);

}

void Game::Quit()
{
	for (auto &car : m_cars) {
		car->Kill();
		delete car;
	}
}

void Game::setLevel(const Level& level)
{
	m_level = level;
}

Level Game::getLevel() const
{
	return m_level;
}

vector<t_line> Game::getCPs() const
{
	return m_cps;
}

vector<t_line> Game::getWalls() const
{
	return m_walls;
}

void Game::addCar(Car* car)
{
	m_cars.push_back(car);
}

void Game::setCars(const vector<Car*> &cars)
{
	m_cars = cars;
}

vector<Car*> Game::getCars() const
{
	return m_cars;
}

void Game::clearCars()
{
	for (auto &car : m_cars) {
		car->Kill();
		delete car;
	}
	m_cars.clear();
}

sf::RenderWindow* Game::getRenderWindow() const
{
	return m_rWnd;
}



Evolution_Controller::Evolution_Controller()
	: m_curr_id(0), m_generation(1), m_best_fitness(0.0f)
{
	
}

bool Evolution_Controller::Init(sf::RenderWindow * rWnd)
{
	if (!Game::Init(rWnd))
		return false;

	this->GenerateRandPopulation();
	return true;
}

void Evolution_Controller::Update(float dt)
{
	

	bool someAlive = false;
	for (auto &car : m_cars)
		if (car->isAlive())
		{
			someAlive = true;
			break;
		}

	Game::Update(dt);
	if (someAlive) 
		return;

	//reset
	for (unsigned i = 0; i < m_cars.size(); ++i)
	{
		m_genomes[i].fitness = m_cars[i]->getScore();
	}

	this->BreedPopulation();

	for (unsigned i = 0; i < m_cars.size(); ++i) //reset cars
	{
		m_NNets[i].setAllWeights(m_genomes[i].weights);
		m_cars[i]->Init(this, m_level.getSpawnPos(), m_level.getSpawnAng());
	}
	
}

void Evolution_Controller::GenerateRandPopulation()
{
	// init the topology
	vector<unsigned> hiddenTopology;
	hiddenTopology.push_back(4);
	hiddenTopology.push_back(3);

	m_pNNTopology = make_unique<NNTopology>(NNTopology(5, hiddenTopology, 2));

	// init genomes
	m_genomes.resize(100); //there will be that many cars per generation

	for (unsigned i = 0; i < m_genomes.size(); ++i)
	{
		t_genome &genome = m_genomes[i];

		genome.index = i;
		genome.id = ++m_curr_id;
		genome.fitness = 0.f;

		genome.weights.resize(m_pNNTopology->getNumConnections());

		for (auto &weight : genome.weights)
			weight = RRand(-1.f, 1.f);
	}

	m_NNets.reserve(m_genomes.size());
	for (unsigned i = 0; i < m_genomes.size(); ++i)
	{
		m_NNets.push_back(NNet(*m_pNNTopology));
		m_NNets[i].setAllWeights(m_genomes[i].weights);
		AICar* pCar = new AICar;
		if (!pCar->Init(this, m_level.getSpawnPos(), m_level.getSpawnAng(), &m_NNets[i]))
			return;

		m_cars.push_back(pCar);
	}

	m_generation = 1;
	m_best_fitness = 0.f;
}

void Evolution_Controller::BreedPopulation()
{
	if (m_genomes.empty())
		return;

	vector<t_genome> bestGenomes;
	bestGenomes = getBestGenomes(10);

	vector<t_genome> children;
	children.reserve(m_genomes.size());

	unsigned curIndex = 0;
	t_genome bestDude;
	if (bestGenomes[0].fitness > m_alpha_genome.fitness) //improvemnt over prev gens
	{
		m_alpha_genome = bestGenomes[0]; m_best_fitness = m_alpha_genome.fitness;
		bestDude.fitness = 0.f; bestDude.id = m_alpha_genome.id; bestDude.fitness = m_alpha_genome.fitness;
		bestDude.weights = m_alpha_genome.weights; bestDude.index = curIndex++;
	}
	else {
		m_best_fitness = bestGenomes[0].fitness;
		bestDude.fitness = 0.f; bestDude.id = bestGenomes[0].id; bestDude.fitness = bestGenomes[0].fitness;
		bestDude.weights = bestGenomes[0].weights; bestDude.index = curIndex++;
	}

	//mutate(bestDude); //TODO change the t_genome into class with .mutate
	children.push_back(bestDude);

	for (unsigned i = 0; i < bestGenomes.size(); ++i)
		for (unsigned j = i + 1; j < bestGenomes.size(); ++j)
		{
			t_genome baby1, baby2;

			CrossBreed(bestGenomes[i], bestGenomes[j], baby1, baby2);
			mutate(baby1);
			mutate(baby2);

			baby1.index = curIndex++;
			baby2.index = curIndex++;

			children.push_back(baby1);
			children.push_back(baby2);

		}

	unsigned remainingPlaces = m_genomes.size() - children.size();

	for (unsigned i = 0; i < remainingPlaces; ++i)
	{
		t_genome gen; gen.id = m_curr_id++;
		gen.fitness = 0.f;
		gen.weights.resize(m_pNNTopology->getNumConnections());

		for (auto &weight : gen.weights)
			weight = RRand(-1.f, 1.f);

		gen.index = curIndex++;
		children.push_back(gen);
	}

	m_genomes = children;
	++m_generation;
	cout << "New generation: " << m_generation << " Best Fit: " << m_best_fitness << " Cur fit: " << bestDude.fitness << endl;

}

void Evolution_Controller::CrossBreed(const t_genome & g1, const t_genome & g2, t_genome & baby1, t_genome & baby2)
{
	unsigned totalCons = g1.weights.size();
	unsigned crossover = min(RRand(0.f, 1.f), 0.99f) * totalCons;

	baby1.id = m_curr_id++;
	baby1.weights.resize(totalCons);

	baby2.id = m_curr_id;
	baby2.weights.resize(totalCons);

	for (unsigned i = 0; i < crossover; ++i)
	{
		baby1.weights[i] = g1.weights[i];
		baby2.weights[i] = g2.weights[i];
	}

	for (unsigned i = crossover; i < totalCons; ++i)
	{
		baby1.weights[i] = g2.weights[i];
		baby2.weights[i] = g1.weights[i];
	}
}

void Evolution_Controller::RateGenome(const unsigned &index, const float &score)
{
	m_genomes[index].fitness = score;
}



void Evolution_Controller::mutate(t_genome& gen)
{
	for (auto &weight : gen.weights)
		if (RRand(0.f,1.f) < 0.1f)//then mutate the weight
			weight += RRand(-0.2f, 0.2f);
}


vector<Evolution_Controller::t_genome> Evolution_Controller::getBestGenomes(const unsigned num)
{
	unsigned _num = min(num, m_genomes.size());

	vector<t_genome> best;
	while(best.size() < _num)
	{
		float bestFitness = -1.f;
		int bestIndex = -1;
		for (unsigned i = 0; i < m_genomes.size(); ++i)
			if (m_genomes[i].fitness > bestFitness)
			{
				bool isUsed = false;

				for (unsigned j=0; j < best.size(); ++j)
					if (best[j].id == m_genomes[i].id)
					{
						isUsed = true;
						break;
					}

				if (isUsed)
					continue;

				bestIndex = i;
				bestFitness = m_genomes[bestIndex].fitness;
			}

		if (bestIndex != -1)
			best.push_back(m_genomes[bestIndex]);
	}

	//reverse(best.begin(), best.end());
	return best; 
}



