#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window.hpp>
#include "Bike.h"
#include "Game.h"

class Player {
	public:
		sf::Window *window;
		sf::Keyboard::Key
			controlKeyLeft,
			controlKeyRight;

		Player(Game *game, int bikeIDArg);
		~Player();

		void setControls(sf::Keyboard::Key left, sf::Keyboard::Key right);
		void onNewGame();
		void turnBike(bool right);
		void drawWindow();

	private:
		Game *game;
		int bikeID, viewedBikeID;
		float yRot;

		void drawBikeAndWalls(Bike *bike);
		void drawFloorAndBorders();
		void drawScene();
};

#endif // PLAYER_H

