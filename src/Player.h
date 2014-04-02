/* Player.h
 * Handles a window and keyboard inputs to control a bike.
 */
#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window.hpp>
#include "Bike.h"
#include "GameInterface.h"
#include "Controller.h"

class Player: public Controller {
	public:
		sf::Window *window;
		sf::Keyboard::Key
			controlKeyLeft,
			controlKeyRight;

		Player(GameInterface *game, int controllerID, Player **players);
		~Player();

		void setControls(sf::Keyboard::Key left, sf::Keyboard::Key right);
		void onNewGame();
		void turnBike(bool right);
		void updateControls();
		void updateView(float frameSec);

	private:
		GameInterface *game;
		Player **players;
		int controllerID, viewedBikeID;
		float yRot;
		bool turnedThisTick;

		bool canTurn(bool right);
		bool canGoForward();
		void drawBikeAndWalls(Bike *bike);
		void drawFloorAndBorders();
		void drawScene();
};

#endif // PLAYER_H

