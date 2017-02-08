#include <SFML/Graphics.hpp>
#include <iostream>

#include "Game.h"

using namespace std;
using namespace sf;

int main()
{
	RenderWindow rWind(VideoMode(960, 1377), "Racing game", sf::Style::Titlebar | sf::Style::Close);

	Evolution_Controller game;
	if (!game.Init(&rWind))
		cerr << "Game init error!";


	Clock deltaTime;
	float dt = 0.000001f;

	srand(time(nullptr));
	
	while (rWind.isOpen())
	{

		// event handling
		sf::Event event;
		while(rWind.pollEvent(event))
		{
			switch(event.type)
			{
			case sf::Event::Closed:
				rWind.close();
				break;

			default: ;
			}
				
		}

		game.EventHandle();

		// clear
		rWind.clear();
		
		//update
		game.Update(30.f*dt);

		//draw
		game.Render();

		//push the frame
		rWind.display();

		//dt
		dt = deltaTime.restart().asSeconds();
		//cout << 1 / dt << endl;
	}

	game.Quit();

	return 0;
}