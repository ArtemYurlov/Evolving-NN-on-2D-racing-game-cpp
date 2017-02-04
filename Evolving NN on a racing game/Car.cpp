#include "Car.h"
#include "Utils.h"
#include "Game.h"
#include <SFML/OpenGL.hpp>

#include <iostream>

Car::Car()
{
	game = nullptr;
	m_pos = sf::Vector2f(0, 0);
	m_color = sf::Color::Blue;
	m_turn = 0;
	m_ang = 0;
	m_speed = 0;
	m_acc = 0;
	m_alive = false;
	m_fric = 0;
	m_maxAcc = 800.f;
	m_fric = 0.001f;
	m_turnCoef = 0.0055f;
}

bool Car::Init( Game* game, const sf::Vector2f& spawnPos, const float& spawnAng)
{
	this->game = game;
	m_pos = spawnPos;
	m_ang = spawnAng;

	m_alive = true;

	return true;
}

void Car::Draw()
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

	sf::Color _col = m_alive ? m_color : sf::Color::Red;
	
	glLineWidth(5.f);

	drawConnected(game->getRenderWindow(), vertices, true, _col, _col);

	glLineWidth(1.f);


}

void Car::Kill()
{
	m_alive = false;
}


PlayerCar::PlayerCar():Car()
{
	
}


sf::Vector2f Car::GetDirectionFacing() const
{
	return sf::Vector2f(cosf(m_ang*PI/180.f), sinf(m_ang*PI/180.f));
}


void Car::EventClear()
{
	m_acc = 0;
	m_turn = 0;
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
	

	// update facing angle
	if (m_turn == -1)
		m_ang -= 90.f*dt*m_speed*m_turnCoef;
	else if (m_turn == 1)
		m_ang += 90.f*dt*m_speed*m_turnCoef;
	
	// get direction facing
	sf::Vector2f dir = this->GetDirectionFacing();
	// update position
	sf::Vector2f prevPos = m_pos;
		// x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt*dt
	m_pos = m_pos + dir * (m_speed - m_fric*m_speed) * dt
		+ dir * 0.5f * float(m_acc)*m_maxAcc*dt*dt;

	m_vel = (m_pos - prevPos) / dt; //update velocity and speed
	m_speed = Norm(m_vel)*sgn(Dot(m_vel, dir)); //signed speed

	this->EventClear();
}

