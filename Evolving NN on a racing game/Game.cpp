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
	/*Car* pCar = new PlayerCar;
	if (!pCar->Init(this, level.getSpawnPos(), level.getSpawnAng()))
		return false;
		
	m_cars.push_back(pCar);*/
	
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
		AICar* pCar = new AICar();
		pCar->Init(this, m_level.getSpawnPos(), m_level.getSpawnAng());
		m_cars.push_back(pCar);
	}

	return true;
}

void Evolution_Controller::Update(float dt)
{
	bool someAlive = false;
	float bestScore = 0.f;
	for (auto &car : m_cars)
		if (car->isAlive())
		{
			if (car->getScore() > bestScore) bestScore = car->getScore();
			someAlive = true;
			break;
		}

	if (!someAlive) // then the generation's run is compleate
	{
		//NNet* nN = static_cast<AICar*>(m_cars[0])->getNNetPtr();

		// Sort cars by score in decreasing order
		sort(m_cars.begin(), m_cars.end(), [](Car* c1, Car* c2) {return c1->getScore() > c2->getScore(); });

		// Give top 40% another run
		unsigned iCar = 0;
		for (; float(iCar) / float(m_cars.size()) < 0.2f; ++iCar)
		{
			static_cast<AICar*>(m_cars[iCar])->getNNetPtr()->setAllWeights( mutate(static_cast<AICar*>(m_cars[iCar])->getNNetPtr()->getAllWeights(), float(iCar-3)/m_cars.size(), 0.1f) );
		}
		// 
		{
			// Get children of the best (in weights of their neural networks)
			vector<vector<float>> children;

			for (unsigned a = 0; a <= iCar; ++a)
				for (unsigned b = a + 1; b <= iCar; ++b)
					children.push_back(productMean_rand(static_cast<AICar*>(m_cars[a])->getNNetPtr()->getAllWeights()
						, static_cast<AICar*>(m_cars[b])->getNNetPtr()->getAllWeights(), 0.05f));

			// Now add the randoms
			for (; float(iCar) / float(m_cars.size()) < 0.25f; ++iCar)
			{
				vector<float> randoms; randoms.resize(static_cast<AICar*>(m_cars[iCar])->getNNetPtr()->getAllWeights().size());
				for (auto &val : randoms)
					val = RRand(-1.f, 1.f);

				static_cast<AICar*>(m_cars[iCar])->getNNetPtr()->setAllWeights(randoms);
			}
			
			// Now fill-in the space with children
			unsigned iChild = 0;
			for (; iCar < m_cars.size(); ++iCar)
				static_cast<AICar*>(m_cars[iCar])->getNNetPtr()->setAllWeights(mutate(children[iChild++]));
			
		}

		cout << "best score: " << m_cars[0]->getScore() << endl;

		for (unsigned i = 0; i < m_cars.size(); ++i) //revive all cars
		{
			m_cars[i]->setRuns(1);
			m_cars[i]->Revive();
		}

		
	}

	Game::Update(dt);
}



