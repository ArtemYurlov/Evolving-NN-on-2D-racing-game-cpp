#include "NNet.h"

NNTopology::NNTopology(const unsigned int numInput, const vector<unsigned int>& vecNumHiddens, const unsigned int numOutput)
	: m_numInput(numInput), m_vecNumHiddens(vecNumHiddens), m_numOutput(numOutput), m_numCons(0.f)
{
	// error handaling
	if (numInput == 0u)
		throw invalid_argument("invalid num of input neurons");

	for (auto &numHid : m_vecNumHiddens)
		if (numHid == 0u)
			throw invalid_argument("invalid number of neurons in a hidden layer");

	if (m_numOutput == 0u)
		throw invalid_argument("invalid num of output neurons");

	// find the total number of connections
	unsigned int prevNumNeurons = m_numInput;
	for (auto& numHidNeurons : m_vecNumHiddens)
	{
		m_numCons += prevNumNeurons * numHidNeurons;
		prevNumNeurons = numHidNeurons;
	}

	m_numCons += prevNumNeurons * m_numOutput;
}

NNet::NNet(const NNTopology & topology)
	: m_topology(topology)
// init the NNet
{
	m_lInput.resize(m_topology.getNumInput()); //set the number of input neurons

	unsigned prevNumNeuron = m_lInput.size();

	m_lHiddens.resize(m_topology.getHiddens().size()); // set the number of hidden layers
	for (unsigned i = 0; i < m_lHiddens.size(); ++i) // init hidden layers
	{
		Layer &layer = m_lHiddens[i];

		layer.resize(m_topology.getHiddens()[i]); //set the number of neurons in the layer
		for (Neuron &n : layer) // init neurons in this layer
		{
			//set the function of the neuron
			//...

			//set the number of connections / weights
			n.setNumWeights(prevNumNeuron); // we want connections to all previous layer's neurons

			vector<float> weights; weights.resize(prevNumNeuron);
			for (auto &weight : weights)
				weight = RRand(-1.f, 1.f);
			n.setWeights(weights);// init weights with random float between -1, 1
		}

		prevNumNeuron = m_topology.getHiddens()[i]; //update the variable for the next iteration
	}

	m_lOutput.resize(m_topology.getNumOutput()); // set the unmber of ourput neurons
	for (Neuron &n : m_lOutput) //init the output layer
	{
		vector<float> weights; weights.resize(prevNumNeuron);
		for (auto &weight : weights)
			weight = RRand(-1.f, 1.f);
		n.setWeights(weights);// init weights with random float between -1, 1
	}
}



NNet::~NNet()
{
}

void NNet::FeedForward(const vector<float>& input, vector<float>& output) const
{
	output.clear();

	// neuron_out = sum( scaled input ), with scaled input = input * weight of the connection;
	// so iterate over each layer saving the outputs in layerOutput
	vector<float> layerInput = input;
	vector<float> layerOutput;

	for(unsigned iLe = 0; iLe < m_lHiddens.size(); ++iLe)
	{
		Layer curLayer = m_lHiddens[iLe];

		for (unsigned i = 0; i < curLayer.size(); ++i) // let each neuron process the inputs and store them in layerOutut in order
		{
			layerOutput.push_back(curLayer[i].Process(layerInput));
		}

		layerInput = layerOutput; // keep doing this untill we reach the last hidden layer
		layerOutput.clear();
	}

	// now get the output from the outputLayer
	for (unsigned i = 0; i < m_lOutput.size(); ++i) // let each neuron process the inputs and store them in layerOutut in order
		layerOutput.push_back(m_lOutput[i].Process(layerInput));

	output = layerOutput; //return the output
}

/*void NNet::setAllWeights(const vector<vector<vector<float>>> &layerNuronWeights)
{
	for (unsigned iL = 0; iL < m_lHiddens.size(); ++iL)
		for (unsigned iN = 0; iN < m_lHiddens[iL].size(); ++iN)
			m_lHiddens[iL][iN].setWeights(layerNuronWeights[iL][iN]);
	for (unsigned iN = 0; iN < m_lOutput.size(); ++iN)
		m_lOutput[iN].setWeights(layerNuronWeights.back()[iN]);
}

vector<vector<vector<float>>> NNet::getAllWeights() const
{
	vector<vector<vector<float>>> allWeights;
	vector<vector<float>> levelWeights;
	for (unsigned iL = 0; iL < m_lHiddens.size(); ++iL)
	{
		for (unsigned iN = 0; iN < m_lHiddens[iL].size(); ++iN)
			levelWeights.push_back(m_lHiddens[iL][iN].getWeights());

		allWeights.push_back(levelWeights);
		levelWeights.clear();
	}

	for (unsigned iN = 0; iN < m_lOutput.size(); ++iN)
		levelWeights.push_back(m_lOutput[iN].getWeights());
	allWeights.push_back(levelWeights);

	return allWeights;
}*/

void NNet::setAllWeights(const vector<float>& layerNeuronWeights)
{
	if (layerNeuronWeights.size() != m_topology.getNumConnections())
		throw invalid_argument("Invalid number of weights to set!");

	// set all the hidden layers
	unsigned iW = 0;
	for (unsigned l = 0; l < m_lHiddens.size(); ++l)
		for (unsigned n = 0; n < m_lHiddens[l].size(); ++n)
		{
			vector<float> weights;
			for (unsigned w = 0; w < m_lHiddens[l][n].getWeights().size(); ++w)
				weights.push_back(layerNeuronWeights[iW++]);
			m_lHiddens[l][n].setWeights(weights);
		}

	// set output layer
	for (unsigned n = 0; n < m_lOutput.size(); ++n)
	{
		vector<float> weights;
		for (unsigned w = 0; w < m_lOutput[n].getWeights().size(); ++w)
			weights.push_back(layerNeuronWeights[iW++]);
		m_lOutput[n].setWeights(weights);
	}
}

vector<float> NNet::getAllWeights() const
{
	vector<float> allWeights;
	allWeights.reserve(m_topology.getNumConnections());

	for (unsigned l = 0; l < m_lHiddens.size(); ++l)
		for (unsigned n = 0; n < m_lHiddens[l].size(); ++n)
		{
			vector<float> weights = m_lHiddens[l][n].getWeights();
			for (unsigned w = 0; w < weights.size(); ++w)
				allWeights.push_back(weights[w]);
		}

	// set output layer
	for (unsigned n = 0; n < m_lOutput.size(); ++n)
	{
		vector<float> weights = m_lOutput[n].getWeights();
		for (unsigned w = 0; w < weights.size(); ++w)
			allWeights.push_back(weights[w]);
	}
	return allWeights;

}
