#ifndef PLAYER_H
#define PLAYER_H

#include <GLFW/glfw3.h>
#include "Bike.h"

class Player {
	public:
		GLFWwindow *window;
		Bike *bike;
		int viewedBikeID;
		float yRot;

		Player(int bikeID);
		~Player();

		void turnBike(bool right);
		void drawWindow();
};

#endif // PLAYER_H

