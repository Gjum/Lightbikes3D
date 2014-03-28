#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>
#include "Player.h"
#include "Bike.h"
#include "Settings.h"

std::vector<Bike *> bikes;
Player *players[2];

///// game logic /////

void newGame() {
	printf("Game restart\n");
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = bikes.at(i);
		// TODO better bike initialization
		bike->pos.x = mapSizeX * (i+1) / (bikesNum+1);
		bike->pos.z = mapSizeZ - bikeRadius;
		bike->direction = 0;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		bike->resetWalls();
		bike->setColor((i+1)   %2,
		              ((i+1)/2)%2,
		              ((i+1)/4)%2);
	}
	players[0]->viewedBikeID = 0;
	players[1]->viewedBikeID = 1;
}

int getBikeID(Bike *bike) {
	for (int i = 0; i < bikes.size(); i++)
		if (bikes.at(i) == bike) return i;
	return EXIT_FAILURE;
}

void testForGameOver() {
	int bikesLeft = 0;
	int lastLivingBike = -1;
	for (int i = 0; i < bikes.size(); i++) {
		if (!bikes.at(i)->isDying()) {
			bikesLeft++;
			if (bikesLeft >= 2) return;
			lastLivingBike = i;
		}
	}
	if (bikesLeft < 2) {
		if (lastLivingBike == -1) printf("Tie!\n");
		else printf("Bike %i wins!\n", lastLivingBike);
		newGame();
	}
}

void killBike(Bike *bike) {
	if (bike->isDying()) return;
	printf("Bike %i crashed\n", getBikeID(bike));
	bike->wallHeight = 0.9999;
}

bool collideBikeWithEverything(Bike *bike) {
	if (bike->collideWithMapBorder())
		return true;
	for (int i = 0; i < bikes.size(); i++) {
		Bike *otherBike = bikes.at(i);
		if (otherBike->isDead()) continue;
		if (bike->collideWithWalls(otherBike))
			return true;
		if (otherBike != bike && bike->collideWithBike(otherBike))
			return true;
	}
	return false;
}

void collideAllBikes() {
	for (int i = 0; i < bikes.size(); i++)
		if (collideBikeWithEverything(bikes.at(i)))
			killBike(bikes.at(i));
}

int goToLivingBike(int start, bool next) {
	int tries = 0; // loop would be infinite if all bikes are dead, but game should have ended then
	int newBikeID = start;
	do {
		if (tries++ > bikes.size()) {
			printf("ERROR goToLivingBike got into an infinite loop\n");
			return start;
		}
		newBikeID += next ? 1 : bikes.size()-1;
		newBikeID %= bikes.size();
	} while (bikes.at(newBikeID)->isDead());
	return newBikeID;
}

void aiTick(float sec) {
	// TODO control the bots
	// start at 2, because 0 and 1 are the players
	for (int i = 2; i < bikes.size(); i++) {
		Bike *bike = bikes.at(i);
		if (bike->isDying()) continue;
		Bike *ghostA = new Bike(bike);
		bike->wallHeight = 0; // simulate death to be ignored on collision tests
		bool right, turn = false;
		// move twice to avoid frontal crash with other bike
		ghostA->move(sec);
		ghostA->move(sec);
		if (collideBikeWithEverything(ghostA) || ghostA->collideWithWalls(ghostA)) {
			// bike is about to crash, but could dodge
			turn = true;
			right = true;
			// TODO fix turning to random direction
//			right = random()%2 == 0; // 1:1 left or right
//			// is it safe to move into this direction?
//			Bike *ghostB = new Bike(bike);
//			ghostB->turn(right);
//			ghostB->move(sec);
//			if (collideBikeWithEverything(ghostB) || ghostB->collideWithWalls(ghostB)) {
//				// no, it's not safe this way, turn the other way
//				right = !right;
//			}
//			delete ghostB;
		}
		delete ghostA;
		bike->wallHeight = 1;
		if (turn) bike->turn(right);
	}
}

///// OpenGL and GLFW /////

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_SPACE:
			case GLFW_KEY_ENTER:
				newGame();
				break;
			case GLFW_KEY_A:
				players[0]->turnBike(false);
				break;
			case GLFW_KEY_D:
				players[0]->turnBike(true);
				break;
			case GLFW_KEY_LEFT:
				players[1]->turnBike(false);
				break;
			case GLFW_KEY_RIGHT:
				players[1]->turnBike(true);
				break;
		}
	}
}

void drawBikeAndWalls(Bike *bike) {
	glColor3fv(bike->color);
	// TODO fancy bike
	if (!bike->isDying()) {
		glPushMatrix();
		glTranslatef(bike->pos.x, 0, bike->pos.z);

		glBegin(GL_QUADS);

		glVertex3f( 0.5, 1,  0.5);
		glVertex3f(-0.5, 1,  0.5);
		glVertex3f(-0.5, 1, -0.5);
		glVertex3f( 0.5, 1, -0.5);

		glVertex3f( 0.5, 0.001,  0.5);
		glVertex3f(-0.5, 0.001,  0.5);
		glVertex3f(-0.5, 0.001, -0.5);
		glVertex3f( 0.5, 0.001, -0.5);

		glEnd();

		glPopMatrix();
	}
	// walls
	if (!bike->isDead()) {
		glBegin(GL_QUADS);

		Point wa = bike->pos, wb;
		for (int i = bike->walls.size() - 1; i >= 0; i--) {
			wb = bike->walls.at(i);
			float ww = ((wa.x < wb.x) ? wa.x : wb.x) - wallRadius; // west
			float we = ((wa.x > wb.x) ? wa.x : wb.x) + wallRadius; // east
			float wn = ((wa.z < wb.z) ? wa.z : wb.z) - wallRadius; // north
			float ws = ((wa.z > wb.z) ? wa.z : wb.z) + wallRadius; // south

			float bh = bike->wallHeight;

			// western wall
			glVertex3f(ww,  0, wn);
			glVertex3f(ww, bh, wn);
			glVertex3f(ww, bh, ws);
			glVertex3f(ww,  0, ws);

			// eastern wall
			glVertex3f(we,  0, wn);
			glVertex3f(we, bh, wn);
			glVertex3f(we, bh, ws);
			glVertex3f(we,  0, ws);

			// northern wall
			glVertex3f(ww,  0, wn);
			glVertex3f(ww, bh, wn);
			glVertex3f(we, bh, wn);
			glVertex3f(we,  0, wn);

			// southern wall
			glVertex3f(ww,  0, ws);
			glVertex3f(ww, bh, ws);
			glVertex3f(we, bh, ws);
			glVertex3f(we,  0, ws);

			// top of the wall
			glVertex3f(ww, bh, wn);
			glVertex3f(ww, bh, ws);
			glVertex3f(we, bh, ws);
			glVertex3f(we, bh, wn);

			wa = wb;
		}

		glEnd();
	}
}

void drawFloorAndBorders() {
	glPushMatrix();
	glBegin(GL_QUADS);

	// floor
	glColor3f(0.2, 0.2, 0.2);

	glVertex3f(       0, 0, 0);
	glVertex3f(       0, 0, mapSizeZ);
	glVertex3f(mapSizeX, 0, mapSizeZ);
	glVertex3f(mapSizeX, 0, 0);

	// borders
	glColor3f(0.3, 0.3, 0.3);

	glVertex3f(       0, 0, 0);
	glVertex3f(       0, 1, 0);
	glVertex3f(mapSizeX, 1, 0);
	glVertex3f(mapSizeX, 0, 0);

	glVertex3f(       0, 0, mapSizeZ);
	glVertex3f(       0, 1, mapSizeZ);
	glVertex3f(mapSizeX, 1, mapSizeZ);
	glVertex3f(mapSizeX, 0, mapSizeZ);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 1, mapSizeZ);
	glVertex3f(0, 0, mapSizeZ);

	glVertex3f(mapSizeX, 0, 0);
	glVertex3f(mapSizeX, 1, 0);
	glVertex3f(mapSizeX, 1, mapSizeZ);
	glVertex3f(mapSizeX, 0, mapSizeZ);

	glEnd();
	glPopMatrix();
}

void drawScene() {
	for (int i = 0; i < bikes.size(); i++)
		drawBikeAndWalls(bikes.at(i));
	drawFloorAndBorders();
}

///// main /////

int main() {
	// init GLFW
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) exit(EXIT_FAILURE);

	srand(time(NULL));
	bikes.clear();
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = new Bike();
		bikes.push_back(bike);
	}

	players[0] = new Player(0);
	players[1] = new Player(1);
	glfwSetWindowPos(players[1]->window, 700, 0);

	newGame();
	while (!glfwWindowShouldClose(players[0]->window) && !glfwWindowShouldClose(players[1]->window)) {
		static double lastFrameSec = glfwGetTime();
		float frameSec = glfwGetTime() - lastFrameSec;
		lastFrameSec = glfwGetTime();

		///// game processing /////

		// bike controls
		aiTick(frameSec);
		glfwPollEvents();

		for (int i = 0; i < bikes.size(); i++)
			bikes.at(i)->move(frameSec);
		collideAllBikes();
		testForGameOver();

		// TODO increase/decrease speed
		//if (glfwGetKey(window, GLFW_KEY_UP)) {}

		///// drawing /////

		players[0]->drawWindow();
		players[1]->drawWindow();

		///// fps calculations /////

		static int frames = 0;
		frames++;
		static double lastSecond = glfwGetTime();
		if (glfwGetTime() - lastSecond > 1) {
			// a second passed
			lastSecond++;
			printf("%3i fps\n", frames);
			frames = 0;
		}
	}
	delete players[0];
	delete players[1];
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

