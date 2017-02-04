#include <SFML/Graphics.hpp>
#include <iostream>

#include "Game.h"

using namespace std;
using namespace sf;

int main()
{
	
	RenderWindow rWind(VideoMode(1000, 1000), "Racing game");

	Game game;
	if (!game.Init(&rWind))
		cerr << "Game init error!";


	Clock deltaTime;
	float dt = 0.000001;
	
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
		game.Update(dt);

		//draw
		game.Render();

		//push the frame
		rWind.display();

		//dt
		dt = deltaTime.restart().asSeconds();
	}

	game.Quit();

	return 0;
}