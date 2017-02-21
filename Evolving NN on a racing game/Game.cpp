#include "Game.h"
#include <iostream>


Game::Game()
{
	m_rWnd = nullptr;
	m_scoreForCompleation = 0.f;
	m_sp_scoreForCompleation = 0.f;
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

	m_sp_scoreForCompleation = 100.f;
	m_scoreForCompleation = m_sp_scoreForCompleation;
	
	//Create a car
	//Car* pCar = new PlayerCar;
	//if (!pCar->Init(this, level.getSpawnPos(), level.getSpawnAng()))
	//	return false;
	//	
	//m_cars.push_back(pCar);
	
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
{
	
}

bool Evolution_Controller::Init(sf::RenderWindow * rWnd)
{
	if (!Game::Init(rWnd))
		return false;

	unsigned _numCars = 100;
	for (unsigned i = 0; i < _numCars; ++i)
	{
		AICar* car = new AICar;
		car->Init(this, m_level.getSpawnPos(), m_level.getSpawnAng());

		Replicator* rep = new Replicator();
		rep->Init(this, car);
		m_replicators.push_back(rep);
	}

	return true;
}

void Evolution_Controller::Update(float dt)
{
	for (auto &rep : m_replicators)
		rep->Update(dt);


	bool someAlive = false;
	for (unsigned i = 0; i < m_replicators.size(); ++i)
		if (m_replicators[i]->isAlive())
		{
			someAlive = true;
			break;
		}

	if (!someAlive) // then the generation's run is compleate
	{

		// Sort cars by score in decreasing order
		sort(m_replicators.begin(), m_replicators.end(), 
			[](Replicator* r1, Replicator* r2) {return r1->getFitness() > r2->getFitness(); });

		// Let the top performers reproduce ( num = sqrt(n) + 1 )
		vector<Replicator*> children; 
		unsigned iRep = 0; //current replicator index
		for (; float(iRep) <= sqrt(float(m_replicators.size())) + 1; ++iRep)
			for (unsigned b = iRep + 1; b <= sqrt(float(m_replicators.size())) + 1; ++b)
				children.push_back( m_replicators[iRep]->Reproduce(m_replicators[b]->getNNWeights()) );

		// Remove/replace the bottom 60% with 50% children from the best and 10% randoms
		{
			// Now add the 10% randoms
			for (; float(iRep) / float(m_replicators.size()) < 0.25f; ++iRep)
			{
				m_replicators[iRep]->Mutate(1.f,1.f,true); //fully mutate
			}
			
			// Now fill-in the space with children
			unsigned iChild = 0;
			for (; iRep < m_replicators.size(); ++iRep)
			{
				m_replicators[iRep] = children[iChild++];
			}

			
		}

		cout << "best score: " << m_replicators[0]->getFitness() << endl;

		for (unsigned i = 0; i < m_replicators.size(); ++i) //reasign and revive all cars
		{
			m_replicators[i]->Revive();
		}

		m_scoreForCompleation = m_sp_scoreForCompleation;

		
	}

}

void Evolution_Controller::Render()
{
	Game::Render();

	for (auto &rep : m_replicators)
		rep->Draw();
}



