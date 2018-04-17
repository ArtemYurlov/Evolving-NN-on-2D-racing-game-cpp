#pragma once

#include <SFML/Graphics.hpp>
#include <math.h>
#include <cstdlib>

using namespace std;

#define PI 3.14159265f

struct t_line
{
	t_line() { p1 = sf::Vector2f(); p2 = sf::Vector2f(); }
	t_line(const sf::Vector2f &p1, const sf::Vector2f &p2) { this->p1 = p1; this->p2 = p2; }
	sf::Vector2f p1;
	sf::Vector2f p2;
};



inline void drawLine(sf::RenderWindow* rWnd, const sf::Vector2f &p1, const sf::Vector2f &p2, const sf::Color &color1 = sf::Color::White, const sf::Color &color2 = sf::Color::White)
{
	sf::Vertex verteces[] = {
		sf::Vertex(p1, color1),
		sf::Vertex(p2, color2)
	};

	rWnd->draw(verteces, 2, sf::Lines);
}

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

	if (vertices.size() <= 1)
		return;

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


inline sf::Vector2f RotateVec2f(const sf::Vector2f &point, const sf::Vector2f &centre, const float angleD)
{
	if (angleD == 0.f)
		return point;
	sf::Vector2f newPoint;

	newPoint.x = centre.x + ((point.x - centre.x) * cosf(angleD*PI / 180.f) - (point.y - centre.y) * sinf(angleD*PI / 180.f));
	newPoint.y = centre.y + ((point.x - centre.x) * sinf(angleD*PI / 180.f) + (point.y - centre.y) * cosf(angleD*PI / 180.f));

	return newPoint;
}

inline float Norm (const sf::Vector2f &vec)
{
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2));
}

inline sf::Vector2f Normalize(const sf::Vector2f &vec)
{
	return vec / Norm(vec);
}

inline sf::Vector2f Perp(const sf::Vector2f &vec)
{
	return sf::Vector2f(vec.y, -1.f * vec.x);
}

inline float Dot(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
	return (v1.x*v2.x) + (v1.y*v2.y);
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}


//collisions

inline float Det(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

inline float CollisionSeqSeq_getPropL1(const sf::Vector2f lineA1, const sf::Vector2f lineA2, const sf::Vector2f lineB1, const sf::Vector2f lineB2)
{
	// returns the proportion up the first line where the collision happens aka returns t, -1.f = no collision

	// let the first like be defined as l1(t) = lineA1 + tr with r:=lineA2 - lineA1 with t in [0,1]
	// simularly l2(s) = lineB1 + us with s:=lineB2-lineB1, u in [0,1]
	const sf::Vector2f r = lineA2 - lineA1;
	const sf::Vector2f s = lineB2 - lineB1;
	// then u will be u= det((lineB1-lineA1),r)/det(r,s)
	const sf::Vector2f o = lineB1 - lineA1;
	const float det1 = Det(o, r);
	const float det2 = Det(r, s);
	//Now if det1 == 0 and det2 == 0 then segments are colinear
	if (det2 == 0)
	{
		if (det1 != 0) // segments are parallel and not intersecting
			return -1.f;
		else // det1 == 0
		{
			const float t0 = Dot(o, r) / Dot(r, r);
			const float t1 = Dot(o + s, r) / Dot(r, r); //= t0+s.r/r.r
			
			if (t0 <= 1.f && t0 >= 0.f && t1 <= 1.f && t1 > 0.f) // then segments overlap
				return t0;
		}
	}
	const float t = Det(o, s) / det2;
	const float u = Det(o, r) / det2;
	if (t <= 1.f && t >= 0.f && u <= 1.f && u > 0.f) // then segments intesect
		return t;
	else //otherwise the segments do not intersect 
		return -1.f;
}

inline sf::Vector2f CollisionSeqSeq(const sf::Vector2f lineA1, const sf::Vector2f lineA2, const sf::Vector2f lineB1, const sf::Vector2f lineB2)
{
	// let the first like be defined as l1(t) = lineA1 + tr with r:=lineA2 - lineA1 with t in [0,1]
	// simularly l2(s) = lineB1 + us with s:=lineB2-lineB1, u in [0,1]
	const sf::Vector2f r = lineA2 - lineA1;
	const sf::Vector2f s = lineB2 - lineB1;
	// then u will be u= det((lineB1-lineA1),r)/det(r,s)
	const sf::Vector2f o = lineB1 - lineA1;
	const float det1 = Det(o, r);
	const float det2 = Det(r, s);
	//Now if det1 == 0 and det2 == 0 then segments are colinear
	if (det2 == 0)
	{
		if (det1 != 0) // segments are parallel and not intersecting
			return sf::Vector2f(0, 0);
		else // det1 == 0
		{
			const float t0 = Dot(o, r) / Dot(r, r);
			const float t1 = Dot(o + s, r) / Dot(r, r); //= t0+s.r/r.r
			if (t0 <= 1.f && t0 >= 0.f && t1 <= 1.f && t1 > 0.f) // then segments overlap
				return lineB1;
		}
	}
	const float t = Det(o, s) / det2;
	const float u = Det(o, r) / det2;
	if (t <= 1.f && t >= 0.f && u <= 1.f && u > 0.f) // then segments intesect
		return lineA1 + t * r;
	else //otherwise the segments do not intersect 
		return sf::Vector2f(0.f, 0.f); 
}

inline float Orient2D (const sf::Vector2f &point, const sf::Vector2f &line_a, const sf::Vector2f &line_b)
{
	// >0 => AB_Point triangle is oriented counterclockwise
	return (line_a.x - point.x) * (line_b.y - point.y) - (line_b.x - point.x) * (line_a.y - point.y);
}

inline bool CollisionDidPointCrossLine(const sf::Vector2f &startPos, const sf::Vector2f &endPos, const sf::Vector2f lineA1, const sf::Vector2f lineA2)
{
	if (Orient2D(startPos, lineA1, lineA2)*Orient2D(endPos, lineA1, lineA2) < 0)
		return true; //means the point crossed AB

	return false;
}

inline bool CollisionDidPointCrossLine(const sf::Vector2f &startPos, const sf::Vector2f &endPos, const t_line &line)
{
	if (Orient2D(startPos, line.p1, line.p2)*Orient2D(endPos, line.p1, line.p2) < 0)
		return true; //means the point crossed AB

	return false;
}

inline sf::Vector2f CollisionSeqSeq(const t_line &l1, const t_line &l2)
{
	return CollisionSeqSeq(l1.p1, l1.p2, l2.p1, l2.p2);
}

inline float RRand(float min, float max) 
// returns random float number in the range between min and max
{
	float rand1 = float(rand()) / float(RAND_MAX) - 0.5f; // a random float between -0.5 and 0.5
	return (max + min)/2 + rand1*(max-min);
}

inline vector<float> productMean_rand(vector<float> v1, vector<float> v2, float randomness)
{
	if (v1.size() != v2.size())
		throw invalid_argument("invalid vector sizes");

	vector<float> vMean;
	for (unsigned i = 0; i < v1.size(); ++i)
	{
		float val;
		if (sgn(v1[i]) == sgn(v2[i]))
			val = float(sgn(v1[i]))*pow(v1[i] * v2[i], 0.5f + RRand(-randomness, randomness));
		else
			val = v1[i];
		vMean.push_back(val);
	}
	return vMean;

}

inline vector<float> x_coross_at(const vector<float> v1, const vector<float> v2, float at)
{
	if (v1.size() != v2.size())
		throw invalid_argument("invalid vector sizes");

	vector<float> vMean;
	unsigned i = 0;
	for (; i < float(v1.size())*at; ++i){
	
		vMean.push_back(v1[i]);
	}

	for (; i < v2.size(); ++i){

		vMean.push_back(v2[i]);
	}
	return vMean;

}


inline vector<float> mutate(vector<float> &v1, const float probOfMutation = 0.07f, const float degreeOfMuataion = 0.5f)
{
	vector<float> _vec(v1);
	for (auto &val : _vec)
		if (RRand(0.f, 1.f) < probOfMutation)
			val += RRand(-abs(degreeOfMuataion * val ), abs(degreeOfMuataion * val));

	return _vec;
}


