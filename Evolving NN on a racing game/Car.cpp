#include "Car.h"
#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>
#include "NNet.h"

Car::Car()
{
	game = nullptr;
	m_color = sf::Color::Blue;
	m_runs = 0;

	mm_w = 10.f;
	mm_h = 20.f;

	m_sp_ang = 0.f;
	m_turn = 0;
	m_ang = 0.f;
	m_speed = 0.f;
	m_bAcc = 0;
	// physics constants
	m_maxAcc = 550.f; //max acceleration of the car
	m_fric = 0.85f; //friction coeff
	m_turnCoef = 0.0055f; //sharpnes of the turn coeff
	
	m_isPlayer = false;

	m_timeDead = 0.f;
	m_reviveIn = -1.f; // negative => do not revive
	m_alive = false;
	m_m_wasDead = false;
	m_score = 0.f;
}

PlayerCar::PlayerCar() :Car()
{
	m_isPlayer = true;
	m_runs = INT_MAX;
	m_reviveIn = 1.f; //revive the car 1 second after the crash
}


bool Car::Init( Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	this->game = game;
	m_sp_pos = spawnPos;
	m_sp_ang = spawnAng;

	m_l_walls = game->getLevel().getWalls();

	return this->Revive(); // updates the car box and draws
}

void Car::Draw() const
{
	//draw the box
	glLineWidth(5.f);

	sf::Color boxCol = m_alive ? m_color : sf::Color::Red;
	drawConnected(game->getRenderWindow(), m_vertices, true, boxCol, boxCol);

	glLineWidth(1.f);

	//draw trail

	sf::Color trailCol = sf::Color::Cyan;
	drawConnected(game->getRenderWindow(), m_trail, false, trailCol, trailCol);

}

void Car::Kill()
{
	m_alive = false;
}

bool Car::Revive()
{
	if (m_runs == 0) //if no runs left dont revive
		return false;

	m_runs--;

	m_pos = m_sp_pos;
	m_ang = m_sp_ang;
	m_vel = sf::Vector2f(0.f, 0.f);
	m_speed = 0;

	m_score = 0u;
	m_checkPointsLeft = game->getCPs(); // update checkpoints
	m_trail.clear();

	m_alive = true;
	m_m_wasDead = true;
	
	m_timeDead = 0.f;

	updateCarBox(); // update the position of the cars box
	this->Draw();

	return true;
}





sf::Vector2f Car::getPos() const
{
	return m_pos;
}

sf::Vector2f Car::getDirectionFacing() const
{
	return sf::Vector2f(cosf(m_ang*PI/180.f), sinf(m_ang*PI/180.f));
}

vector<t_line> Car::getEdges() const
{
	vector<t_line> edges;
	for (unsigned i = 0; i < m_vertices.size() - 1; ++i)
	{
		t_line edge;
		edge.p1 = m_vertices[i];
		edge.p2 = m_vertices[i + 1];
		edges.push_back( edge );
	}
	t_line edge;// last edge
	edge.p1 = m_vertices[3]; 
	edge.p2 = m_vertices[0];
	edges.push_back(edge);
	return edges;
}

vector<sf::Vector2f> Car::getVertices() const
{
	return m_vertices;
}

bool Car::isAlive() const
{
	return m_alive;
}

bool Car::isPlayer() const
{
	return m_isPlayer;
}

void Car::addScore()
{
	m_score++;
}

float Car::getScore() const
{
	return float(m_score);
}

void Car::addRun()
{
	m_runs++;
}

void Car::setRuns(const unsigned & runs)
{
	m_runs = runs;
}

unsigned Car::getRuns() const
{
	return m_runs;
}


void Car::EventClear()
{
	m_bAcc = 0;
	m_turn = 0;
}

void Car::updateCarBox()
{
	const float _h = mm_h;
	const float _w = mm_w;
	
	vector<sf::Vector2f> vertices;
	vertices.push_back(sf::Vector2f(m_pos.x + _h, m_pos.y + _w));//top-left corner
	vertices.push_back(sf::Vector2f(m_pos.x + _h, m_pos.y - _w));//top-right
	vertices.push_back(sf::Vector2f(m_pos.x - _h, m_pos.y - _w));//b-r
	vertices.push_back(sf::Vector2f(m_pos.x - _h, m_pos.y + _w));//b-l
	
	for (auto &vert : vertices)
	vert = RotateVec2f(vert, m_pos, m_ang); // rotate the vertecies to the correct direction of the car
	
	m_vertices = vertices;
}

void Car::Update(float dt)
{
	this->EventHandle();

	if (!m_alive) //dont let to update if dead
	{
		if (m_reviveIn <= 0 || m_timeDead < m_reviveIn) //if we shuld remain dead
		{
			m_timeDead += dt;
			return;
		}
		this->Revive(); //otherwise revive
	}

	// update facing angle
	if (m_turn == -1)
		m_ang -= 90.f*dt*m_speed*m_turnCoef; 
	else if (m_turn == 1)
		m_ang += 90.f*dt*m_speed*m_turnCoef;
	
	// get direction facing
	sf::Vector2f dir = this->getDirectionFacing();
	// update position
		// x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt*dt
	m_acc = dir * float(m_bAcc)*m_maxAcc;
	m_speed = Dot(m_vel, dir);
	m_pos = m_pos + dir * Dot(m_vel, dir) * dt
		+ 0.5f * m_acc * dt * dt;

	m_vel = dir * m_speed + m_acc * dt; //change velocity into the direction of the car
	m_vel += -m_fric * m_vel * dt; // add friction

	vector<sf::Vector2f> prevVerts = m_vertices;
	updateCarBox(); //update the box

	if (!m_m_wasDead)
	{
		// collision update
		if (isAlive())
			for (auto &wall : m_l_walls)
				for (unsigned i = 0; i < m_vertices.size(); ++i)
				{
					if (CollisionDidPointCrossLine(prevVerts[i], m_vertices[i], wall.p1, wall.p2) &&
						CollisionSeqSeq(prevVerts[i], m_vertices[i], wall.p1, wall.p2) != sf::Vector2f())
					{
						this->Kill();
					}
				}

		//update 
		sf::Vector2f prevFrontPos = (prevVerts[0] + prevVerts[1]) / 2.f;
		sf::Vector2f curFrontPos = (m_vertices[0] + m_vertices[1]) / 2.f;
		if (m_alive)
		for (unsigned i = 0; i < m_checkPointsLeft.size(); ++i)
			if (CollisionDidPointCrossLine(prevFrontPos, curFrontPos
				, m_checkPointsLeft[i].p1, m_checkPointsLeft[i].p2)
				&& CollisionSeqSeq(prevFrontPos, curFrontPos
					, m_checkPointsLeft[i].p1, m_checkPointsLeft[i].p2) != sf::Vector2f()) // if the front of the car cross a checkLine
			{
				this->addScore();
				m_checkPointsLeft.erase(m_checkPointsLeft.begin() + i);
			}

		//add trail
		m_trail.push_back((m_vertices[2] + m_vertices[3]) / 2.f);//draw trail from the bottom -middle of the car
	}
	m_m_wasDead = false;

	this->EventClear();
}

//Player Car event handle
void PlayerCar::EventHandle()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_bAcc = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_bAcc = -1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_turn = -1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_turn = 1;
}


//.............. AI Car ................
AICar::AICar() : Car()
{
	m_NN = nullptr;
	m_timeAlive = 0.f;
	m_timeSinceCp = 0.f;
	m_o_acc = 0.f;
	m_o_turn = 0.f;
	m_isPlayer = false;
	m_runs = 1; // give the car 1 initial run at the spawn
}
AICar::~AICar()
{
	delete m_NN;
}

bool AICar::Init(Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	if (!Car::Init(game, spawnPos, spawnAng))
		return false;

	this->updateSensors();

	vector<unsigned> numHiddens;
	for (unsigned i = m_sensors.size() - 1; i > 2; i--)
		numHiddens.push_back(i);

	m_NN = new NNet(*(new NNTopology(m_sensors.size(), numHiddens, 2))); // a convolutional looking topology

	return true;
}

//AI Car event handle


void AICar::updateSensors()
{
	// position and orientation
	//sf::Vector2f front = (m_vertices[0] + m_vertices[1]) / 2.f;
	sf::Vector2f line = sf::Vector2f(500.f, 0.f);
	static const float angles[9] = { 0.f,  -50.f, 50.f,  -25.f , 25.f,  -75.f, 75.f,  -12.5f, 12.5f  };

	for (unsigned i = 0; i < m_sensors.size(); ++i)
	{
		m_sensors[i] = t_sensor(t_line(m_pos, RotateVec2f(line + m_pos, m_pos, m_ang + angles[i])), 1.f);
	}

	// collsiion
	vector<t_line> walls = game->getWalls();
	for (auto &sen : m_sensors)
		for (auto &wall : walls)
		{
			float t = CollisionSeqSeq_getPropL1(sen.line.p1, sen.line.p2, wall.p1, wall.p2);

			if (t != -1.f && t < sen.value)
			{
				sen.value = t;
			}
		}
}

void AICar::EventHandle()
{
	updateSensors();

	// get a move of the NN based on the sensors
	vector<float> input;
	vector<float> output;

	for (unsigned i = 0; i < m_sensors.size(); ++i)
		input.push_back(m_sensors[i].value);

	m_NN->FeedForward(input, output);
	
	float acc = output[0];
	float turn = output[1];
	
	if (isnan(acc)) acc = 0.f;
	if (isnan(turn)) turn = 0.f;

	turn = min<float>(360 / 64, max<float>(-360 / 64, turn));
	m_o_turn = turn;
	float speed = acc;
	speed = min(25.f, max(-10.f, speed));
	m_o_acc = speed;
 


	/* temp controlls
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_bAcc = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_bAcc = -1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_turn = -1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_turn = 1;
	*/
}

void AICar::Update(float dt)
{
	if (!m_alive)
		return;
	m_timeAlive += dt;

	if (m_timeSinceCp > 1.f) //time out
		this->Kill();
	m_timeSinceCp += dt;

	this->EventHandle();

	m_ang += 100.f*m_o_turn*dt;

	m_pos.x += 100.f *m_o_acc * cosf(m_ang*PI / 180.f) * dt;
	m_pos.y += 100.f*m_o_acc * sinf(m_ang*PI / 180.f) * dt;

	vector<sf::Vector2f> prevVerts = m_vertices;
	updateCarBox(); //update the box

	if (!m_m_wasDead)
	{
		// collision update
		if (isAlive())
			for (auto &wall : m_l_walls)
				for (unsigned i = 0; i < m_vertices.size(); ++i)
				{
					if (CollisionDidPointCrossLine(prevVerts[i], m_vertices[i], wall.p1, wall.p2) &&
						CollisionSeqSeq(prevVerts[i], m_vertices[i], wall.p1, wall.p2) != sf::Vector2f())
					{
						this->Kill();
					}
				}

		//update 
		sf::Vector2f prevFrontPos = (prevVerts[0] + prevVerts[1]) / 2.f;
		sf::Vector2f curFrontPos = (m_vertices[0] + m_vertices[1]) / 2.f;
		if (m_alive)
			for (unsigned i = 0; i < m_checkPointsLeft.size(); ++i)
				if (CollisionDidPointCrossLine(prevFrontPos, curFrontPos
					, m_checkPointsLeft[i].p1, m_checkPointsLeft[i].p2)
					&& CollisionSeqSeq(prevFrontPos, curFrontPos
						, m_checkPointsLeft[i].p1, m_checkPointsLeft[i].p2) != sf::Vector2f()) // if the front of the car cross a checkLine
				{
					this->addScore();
					m_checkPointsLeft.erase(m_checkPointsLeft.begin() + i);
				}

		//add trail
		m_trail.push_back((m_vertices[2] + m_vertices[3]) / 2.f);//draw trail from the bottom -middle of the car
	}
	m_m_wasDead = false;

	this->EventClear();
}


void AICar::Draw() const
{
	Car::Draw();

	if (!m_alive)
		return;

	//draw sensors
	for (auto &sen : m_sensors)
	{
		sf::Vector2f coll = sen.line.p1 + (sen.line.p2 - sen.line.p1) * sen.value; // the point of collision between sensor and wall
		sf::Color col = sen.value < 1 ? sf::Color::Yellow : sf::Color::White;

		drawLine(game->getRenderWindow(), sen.line.p1, coll, sf::Color::White, col);
		drawPoint(game->getRenderWindow(), coll, 10, col);
	}
}

void AICar::addScore()
{
	m_score++;
	m_score += 1.f + 0.5f / (1.f + m_timeSinceCp);
	m_timeSinceCp = 0.f;
}

float AICar::getScore() const
{
	//cout << m_score;
	return float(m_score);

}

NNet * AICar::getNNetPtr() const
{
	return m_NN;
}
