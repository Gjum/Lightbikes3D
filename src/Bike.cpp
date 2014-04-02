#include "Bike.h"
#include "Settings.h"
#include <cstdio>

Bike::Bike(Controller *controller) {
	this->controller = controller;
	pos.x = 0;
	pos.z = 0;
	direction = 0;
	wallHeight = 0;
	speed = 0;
	color[0] = 1;
	color[1] = 0;
	color[2] = 1;
	walls.clear();
}

Bike::Bike(Bike *bike) {
	pos        = bike->pos;
	direction  = bike->direction;
	wallHeight = bike->wallHeight;
	speed      = bike->speed;
	color[0]   = bike->color[0];
	color[1]   = bike->color[1];
	color[2]   = bike->color[2];
	walls      = bike->walls;
}

Bike::~Bike() {}

bool Bike::isDying() {
	return wallHeight < 1;
}

bool Bike::isDead() {
	return wallHeight <= 0;
}

Box Bike::getWallBox(int i) {
	Box box;
	if (i < 0 || i >= walls.size()) {
		printf("ERROR: Bike collision test: Invalid wall index %i\n", i);
		return box;
	}
	Point a = (i >= walls.size()-1) ? pos : walls.at(i+1);
	Point b = walls.at(i);
	box.w = (a.x < b.x ? a.x : b.x) - wallRadius;
	box.e = (a.x > b.x ? a.x : b.x) + wallRadius;
	box.n = (a.z < b.z ? a.z : b.z) - wallRadius;
	box.s = (a.z > b.z ? a.z : b.z) + wallRadius;
	return box;
}

Box Bike::getBikeBox() {
	Box b;
	b.w = pos.x - bikeRadius;
	b.e = pos.x + bikeRadius;
	b.n = pos.z - bikeRadius;
	b.s = pos.z + bikeRadius;
	return b;
}

bool Bike::collideWithBike(Bike *otherBike) {
	if (this == otherBike) {
		printf("ERROR: Colliding bike with itself\n");
		return false;
	}
	return collideBoxes(getBikeBox(), otherBike->getBikeBox());
}

bool Bike::collideWithWalls(Bike *otherBike) {
	for (int i = otherBike->walls.size()-1; i >= 0; i--) {
		if (this == otherBike && i > (int)walls.size() - 3) continue;
		if (collideBoxes(getBikeBox(), otherBike->getWallBox(i)))
			return true;
	}
	return false;
}

bool Bike::collideWithMapBorder() {
	return pos.x-bikeRadius < 0
	    || pos.x+bikeRadius > mapSizeX
	    || pos.z-bikeRadius < 0
	    || pos.z+bikeRadius > mapSizeZ;
}

void Bike::resetWalls() {
	walls.clear();
	walls.push_back(pos);
}

void Bike::setColor(float r, float g, float b) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

void Bike::move(float units) {
	direction %= 4;
	switch (direction) {
		case 0:
			pos.z -= units;
			break;
		case 1:
			pos.x += units;
			break;
		case 2:
			pos.z += units;
			break;
		case 3:
			pos.x -= units;
			break;
	}
}

void Bike::onPhysicsTick() {
	// do not move dying bikes, make walls smaller instead
	if (isDying()) {
		wallHeight = wallHeight - physicsTickTime * wallShrinkSpeed;
		if (wallHeight < 0) wallHeight = 0; // prevent overflow
	}
	else move(physicsTickTime * speed);
}

void Bike::turn(bool right) {
	direction = (direction + (right ? 1 : 3)) % 4;
	walls.push_back(pos);
}

