/* Bike.h
 * Contains the state of a Lightcycle and provides methods to change it.
 */
#ifndef BIKE_H
#define BIKE_H

#include <vector>
#include "Controller.h"
#include "Collision.h"

class Bike {
	public:
		Controller *controller;
		Point pos;
		char direction;
		float wallHeight, speed;
		float color[3];
		std::vector<Point> walls;

		Bike(Controller *controller);
		Bike(Bike *bike);
		~Bike();

		bool isDying();
		bool isDead();
		Box getBikeBox();
		Box getWallBox(int wallIndex);
		bool collideWithBike(Bike *otherBike);
		bool collideWithWalls(Bike *otherBike);
		bool collideWithMapBorder();

		void resetWalls();
		void setColor(float r, float g, float b);
		void move(float units);
		void onPhysicsTick();
		void turn(bool right);

	private:
		bool turnedThisTick;
};

#endif // BIKE_H

