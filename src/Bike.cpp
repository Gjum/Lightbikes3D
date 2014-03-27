#include "Bike.h"
#include "Settings.h"

Bike::Bike() {}
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
	// TODO collideWithMapBorder
	return false;
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

void Bike::move(float sec) {
	// do not move dying bikes, make walls smaller instead
	if (isDying()) {
		wallHeight = wallHeight - sec*wallShrinkSpeed;
		if (wallHeight < 0) wallHeight = 0; // prevent overflow
	}
	else {
		switch (direction) {
			case 0:
				pos.z -= sec * speed;
				break;
			case 1:
				pos.x += sec * speed;
				break;
			case 2:
				pos.z += sec * speed;
				break;
			case 3:
				pos.x -= sec * speed;
				break;
		}
	}
}

void Bike::turn(bool right) {
	if (isDying()) return;
	direction = (direction + (right ? 1 : 3)) % 4;
	walls.push_back(pos);
}

