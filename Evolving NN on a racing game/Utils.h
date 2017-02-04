#pragma once

#include <SFML/Graphics.hpp>

using namespace std;

#define PI 3.14159265f

struct t_line
{
	sf::Vector2f p1;
	sf::Vector2f p2;
};

inline void drawLine(sf::RenderWindow* rWnd, const t_line &line, const sf::Color &color1 = sf::Color::White, const sf::Color &color2 = sf::Color::White)
{
	sf::Vertex verteces[] = {
		sf::Vertex(sf::Vector2f(line.p1), color1),
		sf::Vertex(sf::Vector2f(line.p2), color2)
	};

	rWnd->draw(verteces, 2, sf::Lines);
}

inline void drawLines(sf::RenderWindow* rWnd, const vector<t_line> &lines, const sf::Color &color1 = sf::Color::White, const sf::Color &color2 = sf::Color::White)
{
	vector<sf::Vertex> vertices;
	vertices.clear();

	for (auto &line : lines)
	{
		vertices.push_back(sf::Vertex(sf::Vector2f(line.p1), color1)); //pushback each vertex/point
		vertices.push_back(sf::Vertex(sf::Vector2f(line.p2), color2));
	}

	rWnd->draw( &vertices[0], vertices.size(), sf::Lines);
}

inline void drawPoint(sf::RenderWindow* rWnd, const sf::Vector2f &point, const float size = 10.f, const sf::Color &color = sf::Color::White)
{
	sf::Vertex verteces[] = {
		sf::Vertex(sf::Vector2f(point.x - size, point.y - size), color),
		sf::Vertex(sf::Vector2f(point.x + size, point.y + size), color),
		sf::Vertex(sf::Vector2f(point.x + size, point.y - size), color),
		sf::Vertex(sf::Vector2f(point.x - size, point.y + size), color),
	}; // draw a  x  for a point

	rWnd->draw(verteces, 4, sf::Lines);
}

inline void drawConnected(sf::RenderWindow* rWnd, const vector<sf::Vector2f> &vertices, const bool connectLast = false, const sf::Color &color1 = sf::Color::White, const sf::Color &color2 = sf::Color::White)
{
	vector<sf::Vertex> vertices_l; // vertecies of lines to be drawn
	vertices_l.clear();

	for (unsigned i = 0; i < vertices.size() - 1; ++i)
	{
		vertices_l.push_back(sf::Vertex(vertices[i], color1));
		vertices_l.push_back(sf::Vertex(vertices[i+1], color2));
	}
	
	if (connectLast) //if the shape is closed then draw the last line
	{
		vertices_l.push_back(sf::Vertex(vertices[vertices.size()-1], color1));
		vertices_l.push_back(sf::Vertex(vertices[0],color2));
	}

	rWnd->draw(&vertices_l[0], vertices_l.size(), sf::Lines);
}


inline sf::Vector2f rotateVec2f(const sf::Vector2f &point, const sf::Vector2f &centre, const float angleD)
{
	sf::Vector2f newPoint;

	newPoint.x = centre.x + (point.x - centre.x) * cosf(angleD*PI / 180.f) + (point.y - centre.y) * sinf(angleD*PI / 180.f);
	newPoint.y = centre.y + (point.x - centre.x) * sinf(angleD*PI / 180.f) - (point.y - centre.y) * cosf(angleD*PI / 180.f);

	return newPoint;
}

inline float Norm (const sf::Vector2f &vec)
{
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2));
}

inline float Dot(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
	return (v1.x*v2.x) + (v1.y*v2.y);
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}