#pragma once
#include "Utils.h"
#include <functional>
#include <windows.h>
#include <memory>
#include <iostream>

using namespace std;

struct NeuroFunc
{
	static float Sum(const vector<float> &scaledInput) {
		float sum = 0.f;

		for (auto &sIn : scaledInput)
			sum += sIn;

		return sum;
	}
};

class Neuron
{
public:
	Neuron() {m_func = &NeuroFunc::Sum;}
	explicit Neuron(function<float(const vector<float>&)> &func) { m_func = func; }
	explicit Neuron(vector<float> weights) : Neuron() { m_weights = weights; }
	Neuron(vector<float> weights, function<float(const vector<float>&)> &func) { m_weights = weights; m_func = func; }
	Neuron(unsigned numConnections, function<float(const vector<float>&)> &func ) { for (unsigned i = 0; i < numConnections; ++i) m_weights.push_back(RRand(-1.f,1.f)); m_func = func; }
	~Neuron() {}

	float Process(const vector<float> &input) const
	{
		vector<float> scaledInput; //scale the inputs by the 
		for (unsigned iIn = 0; iIn < input.size(); ++iIn)
			scaledInput.push_back(input[iIn] * m_weights[iIn]); 

		return m_func(scaledInput);
	}

	//get/set
	vector<float> getWeights() const { return m_weights; }
	void setWeights(const vector<float> &weights) { m_weights = weights; }
	void setNumWeights(const unsigned numWeights) { m_weights.resize(numWeights); }

	void setFunc(function<float(vector<float>)> &func) { m_func = func; }
	function<float(vector<float>)> getFunc() const { return m_func; }

private:
	vector<float> m_weights;
	function<const float(vector<float>&)> m_func;
};

typedef vector<Neuron> Layer;

class NNTopology
{
public:
	NNTopology(const unsigned int numInput, const vector<unsigned int> &vecNumHiddens, const unsigned int numOutput);

	//get/set
	unsigned getNumInput() const { return m_numInput; }
	unsigned getNumOutput() const { return m_numOutput; }
	vector<unsigned> getHiddens() const { return m_vecNumHiddens; }

	unsigned getNumConnections() const { return m_numCons; }

private:
	unsigned m_numInput;
	vector<unsigned int> m_vecNumHiddens;
	unsigned m_numOutput;

	unsigned m_numCons; // total number of connections between neurons in the topology
};

class NNet
{
public:
	NNet(NNTopology &topology);
	~NNet();

	//methods
	vector<float> FeedForward(const vector<float> &input) const;

	//get/set
	//void setAllWeights(const vector<vector<vector<float>>> &layerNeuronWeights);
	//vector<vector<vector<float>>> getAllWeights() const;

	void setAllWeights(const vector<float> &layerNeuronWeights);
	vector<float> getAllWeights() const;

	NNTopology* getTopology() const;


private:
	NNTopology& m_topology;

	Layer m_lInput;
	vector<Layer> m_lHiddens;
	Layer m_lOutput;
};

