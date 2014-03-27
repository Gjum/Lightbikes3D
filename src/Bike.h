#ifndef BIKE_H
#define BIKE_H

#include <vector>

struct Point {
	float x, z;
};

class Bike {
	public:
		Point pos;
		char direction;
		float wallHeight, speed;
		float color[3];
		std::vector<Point> walls;

		Bike();
		~Bike();

		bool isDying();
		bool isDead();
		bool collideWithBike(Bike *otherBike);
		bool collideWithWalls(Bike *otherBike);
		bool collideWithMapBorder();

		void resetWalls();
		void setColor(float r, float g, float b);
		void move(float sec);
		void turn(bool right);
};

#endif // BIKE_H

