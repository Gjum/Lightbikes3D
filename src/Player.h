#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window.hpp>
#include "Bike.h"

class Player {
	public:
		sf::Window *window;
		int viewedBikeID;
		sf::Keyboard::Key
			controlKeyLeft,
			controlKeyRight;

		Player(int bikeIDArg);
		~Player();

		void setControls(sf::Keyboard::Key left, sf::Keyboard::Key right);
		void onNewGame();
		void turnBike(bool right);
		void drawWindow();

	private:
		int bikeID;
		float yRot;
};

#endif // PLAYER_H

