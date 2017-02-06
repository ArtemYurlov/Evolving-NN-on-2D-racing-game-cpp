#include "Car.h"
#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>
#include <iostream>


Car::Car()
{
	game = nullptr;
	m_color = sf::Color::Blue;

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
	m_score = 0u;
}

PlayerCar::PlayerCar() :Car()
{
	m_isPlayer = true;
	m_reviveIn = 1.f; //revive the car 1 second after the crash
}


bool Car::Init( Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	this->game = game;
	m_sp_pos = spawnPos;
	m_sp_ang = spawnAng;

	this->Revive(); // updates the car box and draws

	return true;
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

void Car::Revive()
{
	m_pos = m_sp_pos;
	m_ang = m_sp_ang;
	m_vel = sf::Vector2f(0.f, 0.f);
	m_speed = 0;

	m_score = 0u;
	m_checkPointsLeft = game->getCPs(); // update checkpoints
	m_trail.clear();

	m_alive = true;
	
	m_timeDead = 0.f;

	updateCarBox(); // update the position of the cars box
	this->Draw();
	cout << "Revived! " << endl;
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

unsigned Car::getScore() const
{
	return m_score;
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

	sf::Vector2f prevFrontPos = (m_vertices[0] + m_vertices[1]) / 2.f;
	updateCarBox(); //update the box

	//update score
	for (unsigned i = 0; i < m_checkPointsLeft.size(); ++i)
		if (CollisionDidPointCrossSeg(prevFrontPos, (m_vertices[0] + m_vertices[1]) / 2.f
			, m_checkPointsLeft[i].p1, m_checkPointsLeft[i].p2)) // if the front of the car cross a checkLine
		{
			this->addScore();
			m_checkPointsLeft.erase(m_checkPointsLeft.begin() + i);
		}
		
	//add trail
	m_trail.push_back((m_vertices[2] + m_vertices[3]) / 2.f );//draw trail from the bottom -middle of the car

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
	m_isPlayer = false;
}

bool AICar::Init(Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	if (!Car::Init(game, spawnPos, spawnAng))
		return false;

	//init sensors
	
	this->updateSensors();
	return true;
}


//AI Car event handle


void AICar::updateSensors()
{
	// position and orientation
	//sf::Vector2f front = (m_vertices[0] + m_vertices[1]) / 2.f;
	sf::Vector2f line = sf::Vector2f(300.f, 0.f);
	static const float angles[3] = { -360.f / 16.f , 0, 360.f / 16.f };

	for (unsigned i = 0; i < m_sensors.size(); ++i)
	{
		m_sensors[i] = t_sensor(t_line(m_pos, RotateVec2f(line + m_pos, m_pos, m_ang + angles[i])), 1.f);
	}

	//cout << m_pos.x << " " << m_pos.y << endl;
	// collsiion
	vector<t_line> walls = game->getWalls();
	for (auto sen : m_sensors)
		for (auto wall : walls)
		{
			float t = CollisionSeqSeq_getPropL1(sen.line.p1, sen.line.p2, wall.p1, wall.p2);

			if (t != -1.f && t < sen.value)
			{
				sen.value = t;
				cout << "sen ";
			}
		}
}

void AICar::EventHandle()
{
	updateSensors();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_bAcc = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_bAcc = -1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_turn = -1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_turn = 1;
	
}

void AICar::Draw() const
{
	Car::Draw();

	//draw sensors
	for (auto &sen : m_sensors)
	{
		//cout << sen.value << " ";
		sf::Vector2f coll = sen.line.p1 + (sen.line.p2 - sen.line.p1) * sen.value; // the point of collision between sensor and wall
		sf::Color col = sen.value < 1 ? sf::Color::Yellow : sf::Color::White;

		cout << "draw" << endl;
		drawLine(game->getRenderWindow(), sen.line.p1, coll, col, col);
		drawPoint(game->getRenderWindow(), coll, 10, col);
	}
}
