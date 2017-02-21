#pragma once
#include "Utils.h"
#include "NNet.h"

class Game;

class AICar;

class Replicator
{
public:
	struct ReplFunctions
	{
		static vector<float> WeightedRandom (const vector<float> &v1, const vector<float> &v2)
		{
			vector<float> out; out = v1;
			for (auto &val : out)
				val += RRand(-1.f, 1.f) * val;

			return out;
		}
	};
public:
	Replicator();
	~Replicator();

	bool Init(Game* game, AICar* car);
	bool Init(Game* game, AICar* car, NNet* nnet);
	bool Init(Game* game, AICar* car, NNet* nnet, vector<float>(*replication_function)(const vector<float>&, const vector<float>&));

	void Update(float dt);

	Replicator* Reproduce(const vector<float> &mate) const;
	void Mutate(const float &prob, const float &amount, const bool &fully = false);

	bool isAlive();
	void Revive();
	void Draw();

	//get/set
	vector<float> getNNWeights() const;
	void setNNWeights(const vector<float> &nnWeights);

	float getFitness() const;


private:
	Game* m_game;
	AICar* m_car;

	NNet* m_NN;

	vector<float> (*m_replication_function)(const vector<float>&, const vector<float>&);
};

