#ifndef COLLISION_H
#define COLLISION_H

struct Point {
	float x, z;
};

struct Box {
	float w, e, n, s; // west, east, north, south
};

bool collideBoxes(const Box &a, const Box &b);

#endif // COLLISION_H

