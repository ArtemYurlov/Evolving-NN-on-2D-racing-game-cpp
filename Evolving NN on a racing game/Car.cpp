#include "Car.h"
#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>

#include <iostream>

Car::Car()
{
	game = nullptr;
	m_color = sf::Color::Blue;

	m_sp_ang = 0.f;
	m_turn = 0;
	m_ang = 0.f;
	m_speed = 0.f;
	m_acc = 0.f;
	// physics constants
	m_maxAcc = 330.f; //max acceleration of the car
	m_fric = 0.0015f; //friction coeff
	m_turnCoef = 0.0075f; //sharpnes of the turn coeff
	

	m_timeDead = 0.f;
	m_reviveIn = -1.f; // negative => do not revive
	m_alive = false;
}

bool Car::Init( Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	this->game = game;
	m_sp_pos = spawnPos;
	m_sp_ang = spawnAng;

	this->Revive();

	return true;
}

void Car::Draw() const
{
	sf::Color _col = m_alive ? m_color : sf::Color::Red;
	
	glLineWidth(5.f);

	drawConnected(game->getRenderWindow(), m_vertices, true, _col, _col);

	glLineWidth(1.f);


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

	updateCarBox(); // update the position of the cars box

	m_alive = true;
	
	m_timeDead = 0.f;
}


PlayerCar::PlayerCar():Car()
{
	m_reviveIn = 1.f; //revive the car 1 second after the crash
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

bool Car::isAlive() const
{
	return m_alive;
}


void Car::EventClear()
{
	m_acc = 0;
	m_turn = 0;
}

void Car::updateCarBox()
{
const float _h = 20.f;
const float _w = 10.f;

vector<sf::Vector2f> vertices;
vertices.push_back(sf::Vector2f(m_pos.x + _h, m_pos.y + _w));
vertices.push_back(sf::Vector2f(m_pos.x + _h, m_pos.y - _w));
vertices.push_back(sf::Vector2f(m_pos.x - _h, m_pos.y - _w));
vertices.push_back(sf::Vector2f(m_pos.x - _h, m_pos.y + _w));

for (auto &vert : vertices)
vert = rotateVec2f(vert, m_pos, m_ang); // rotate the vertecies to the correct direction of the car

m_vertices = vertices;
}


void PlayerCar::EventHandle()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		m_acc = 1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		m_acc = -1;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		m_turn = -1;
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		m_turn = 1;
}


void Car::Update(float dt)
{
	this->EventHandle();

	if (!m_alive) //dont let to update if dead
	{
		if (m_reviveIn >= 0 && m_timeDead > m_reviveIn) //if revive is possible wait for the revive
			this->Revive();

		m_timeDead += dt;
		return;
	}

	// update facing angle
	if (m_turn == -1)
		m_ang -= 90.f*dt*(m_speed)*m_turnCoef; // imared controlls while accelerating
	else if (m_turn == 1)
		m_ang += 90.f*dt*(m_speed)*m_turnCoef;
	
	// get direction facing
	sf::Vector2f dir = this->getDirectionFacing();
	// update position
	sf::Vector2f prevPos = m_pos;
		// x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt*dt
	m_pos = m_pos + dir * (m_speed - m_fric*m_speed) * dt
		+ dir * 0.5f * float(m_acc)*m_maxAcc*pow(dt,1.8f); // dt^1.8 and not 2 is to make the car more reactive

	m_vel = (m_pos - prevPos) / dt; //update velocity and speed
	m_speed = Norm(m_vel)*sgn(Dot(m_vel, dir)); //signed speed

	updateCarBox();

	this->EventClear();
}

