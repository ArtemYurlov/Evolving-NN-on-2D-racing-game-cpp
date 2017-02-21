#include "Replicator.h"
#include "Car.h"



Replicator::Replicator()
	:m_game(nullptr), m_car(nullptr), m_NN(nullptr), m_replication_function(&ReplFunctions::WeightedRandom)
{
}


Replicator::~Replicator()
{
}

bool Replicator::Init(Game * game, AICar* car)
{
	// init the NNet
	vector<unsigned> numHiddens; unsigned numSensors = car->getSensors().size();
	for (unsigned i = numSensors - 1; i > 2; i--)
		numHiddens.push_back(i);
	NNet* nN = new NNet(*new NNTopology(numSensors, numHiddens, 2)); // a convolutional looking topology

	return this->Init(game, car, nN);
}

bool Replicator::Init(Game * game, AICar* car, NNet * nnet)
{
	//init the basic replication function
	auto replication_function = &ReplFunctions::WeightedRandom;


	return this->Init(game, car, nnet, replication_function);
}

bool Replicator::Init(Game * game, AICar* car, NNet * nnet, vector<float>(*replication_function)(const vector<float>&, const vector<float>&))
{
	m_game = game;
	m_car = car;
	m_NN = nnet;

	m_replication_function = replication_function;


	return true;
}

void Replicator::Update(float dt)
{
	//get the cars sensors data
	m_car->updateSensors();
	vector<float> data_sensors = m_car->getSensors();

	//feed the data forward through the NN for the output
	array<float, 2> output = { m_NN->FeedForward(data_sensors)[0],  m_NN->FeedForward(data_sensors)[1] };

	//set controlls of the car to the output of the NN
	m_car->setControls(output[0], output[1]);
	//update the car
	m_car->Update(dt);
}

Replicator* Replicator::Reproduce(const vector<float>& matesWeights) const
{
	Replicator* rep = new Replicator();
	NNet* newNN = new NNet(*m_NN->getTopology());
	newNN->setAllWeights(m_replication_function(newNN->getAllWeights(), matesWeights));
	rep->Init(m_game, m_car, newNN, m_replication_function); //TODO make m_replication_function a NN
	return rep;
}

void Replicator::Mutate(const float & prob, const float & amount, const bool & fully)
{
	vector<float> currWeights = m_NN->getAllWeights();
	for (auto &weight : currWeights)
	{
		if (!fully)
		{
			if (RRand(0.f, 1.f) < prob)
				weight += RRand(-amount, +amount);
		}
		else
			if (RRand(0.f, 1.f) < prob)
				weight = RRand(-amount, amount);
	}

	m_NN->setAllWeights(currWeights);
}

bool Replicator::isAlive()
{
	return m_car->isAlive();
}

void Replicator::Revive()
{
	m_car->setRuns(1);
	m_car->Revive();
}

void Replicator::Draw()
{
	m_car->Draw();
}

vector<float> Replicator::getNNWeights() const
{
	return m_NN->getAllWeights();
}

void Replicator::setNNWeights(const vector<float>& nnWeights)
{
	m_NN->setAllWeights(nnWeights);
}

float Replicator::getFitness() const
{
	return m_car->getScore();
}

