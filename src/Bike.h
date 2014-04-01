#ifndef BIKE_H
#define BIKE_H

#include <vector>
#include "Controller.h"

struct Point {
	float x, z;
};

class Bike {
	public:
		Controller *controller;
		Point pos;
		char direction;
		float wallHeight, speed;
		float color[3];
		std::vector<Point> walls;

		Bike();
		Bike(Bike *bike);
		~Bike();

		bool isDying();
		bool isDead();
		bool collideWithBike(Bike *otherBike);
		bool collideWithWalls(Bike *otherBike);
		bool collideWithMapBorder();

		void resetWalls();
		void setColor(float r, float g, float b);
		void onPhysicsTick();
		void turn(bool right);

	private:
		bool turnedThisTick;
};

#endif // BIKE_H

