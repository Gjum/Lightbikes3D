#include "Bike.h"
#include "Settings.h"

Bike::Bike() {
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

bool Bike::collideWithBike(Bike *otherBike) {
	// TODO collideWithBike
	// TODO print debugging message when colliding bike with itself
	return false;
}

bool Bike::collideWithWalls(Bike *otherBike) {
	Point wa, wb;
	float bw = pos.x - bikeRadius; // west
	float be = pos.x + bikeRadius; // east
	float bn = pos.z - bikeRadius; // north
	float bs = pos.z + bikeRadius; // south
	// do not collide with the two newest walls if bike == otherBike
	if (this == otherBike) {
		if (otherBike->walls.size() < 3) return false;
		wa = otherBike->walls.at(otherBike->walls.size() - 2); // start at third-newest wall
	}
	else wa = otherBike->pos;
	for (int i = otherBike->walls.size() - (this == otherBike ? 3 : 1); i >= 0; i--) {
		wb = otherBike->walls.at(i);
		float ww = ((wa.x < wb.x) ? wa.x : wb.x) - wallRadius; // west
		float we = ((wa.x > wb.x) ? wa.x : wb.x) + wallRadius; // east
		float wn = ((wa.z < wb.z) ? wa.z : wb.z) - wallRadius; // north
		float ws = ((wa.z > wb.z) ? wa.z : wb.z) + wallRadius; // south
		if (ww <= be && we >= bw && wn <= bs && ws >= bn)
			return true;
		wa = wb;
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

void Bike::onPhysicsTick() {
	// do not move dying bikes, make walls smaller instead
	if (isDying()) {
		wallHeight = wallHeight - physicsTickTime * wallShrinkSpeed;
		if (wallHeight < 0) wallHeight = 0; // prevent overflow
	}
	else {
		switch (direction) {
			case 0:
				pos.z -= physicsTickTime * speed;
				break;
			case 1:
				pos.x += physicsTickTime * speed;
				break;
			case 2:
				pos.z += physicsTickTime * speed;
				break;
			case 3:
				pos.x -= physicsTickTime * speed;
				break;
		}
	}
}

void Bike::turn(bool right) {
	direction = (direction + (right ? 1 : 3)) % 4;
	walls.push_back(pos);
}

