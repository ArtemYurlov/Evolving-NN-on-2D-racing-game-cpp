#include "Car.h"

#define PI 3.14159265f

Car::Car()
{
	game = nullptr;
	m_turn = 0;
	m_ang = 0;
	m_speed = 0;
	m_acc = 0;
	m_alive = false;
	m_fric = 0;
	m_maxAcc = 50;
	m_pos = sf::Vector2f(0, 0);
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
	return sf::Vector2f(cos(m_ang*PI/180.f), sin(m_ang*PI/180.f));
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


void PlayerCar::Update(float dt)
{
	this->EventHandle();

	// update facing angle
	if ((m_acc == 1) && (m_turn == -1))
		m_ang -= 45.f*dt;
	else if ((m_acc == 1) && (m_turn == 1))
		m_ang += 45.f*dt;
	else if ((m_acc == -1) && (m_turn == -1))
		m_ang += 45.f*dt;
	else if ((m_acc == -1) && (m_turn == 1))
		m_ang -= 45.f*dt;
	// get direction facing
	sf::Vector2f dir = this->GetDirectionFacing();
	// update position
	sf::Vector2f prevPos = m_pos;
	// x(t+dt) = x(t) + v(t)*dt + 0.5*a(t)*dt*dt
	m_pos = m_pos + dir * (m_speed - m_speed*m_fric)*dt + 0.5f * dir * float(m_acc)*m_maxAcc*dt*dt;
	m_speed = sqrt(pow((m_pos - prevPos).x, 2) + pow((m_pos - prevPos).y, 2))/dt; //update speed

	this->EventClear();
}
