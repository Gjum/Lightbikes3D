#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window.hpp>
#include "Bike.h"
#include "Game.h"
#include "Controller.h"

class Player: public Controller {
	public:
		sf::Window *window;
		sf::Keyboard::Key
			controlKeyLeft,
			controlKeyRight;

		Player(Game *game, int bikeID, Player **players);
		~Player();

		void setControls(sf::Keyboard::Key left, sf::Keyboard::Key right);
		void onNewGame();
		void turnBike(bool right);
		void updateControls();
		void updateView(float frameSec);

	private:
		Game *game;
		Player **players;
		int bikeID, viewedBikeID;
		float yRot;

		void drawBikeAndWalls(Bike *bike);
		void drawFloorAndBorders();
		void drawScene();
};

#endif // PLAYER_H

