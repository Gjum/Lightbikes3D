#include "Collision.h"

bool collideBoxes(const Box &a, const Box &b) {
	return a.w <= b.e && a.e >= b.w && a.n <= b.s && a.s >= b.n;
}

