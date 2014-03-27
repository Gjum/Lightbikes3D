#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include "Bike.h"
#include "Settings.h"

std::vector<Bike *> bikes;
Bike *ownBike = NULL;
int viewedBikeID = 0;

///// game logic /////

void newGame() {
	printf("Game restart\n");
	for (int i = 0; i < bikes.size(); i++)
		delete (bikes.at(i));
	bikes.clear();

	static const int bikesNum = 5;
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = new Bike();
		bikes.push_back(bike);
		// TODO better bike initialization
		bike->pos.x = mapSizeX * (i+1) / (bikesNum+1);
		bike->pos.z = bikeRadius;
		bike->direction = 2;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		bike->resetWalls();
		bike->setColor(i%2,
		               (i/2)%2,
		               (i/4)%2);
	}
	ownBike = bikes.at(0);
	ownBike->pos.z = mapSizeZ - bikeRadius;
	ownBike->direction = 0;
	ownBike->resetWalls();

	viewedBikeID = 0;
}

int getBikeID(Bike *bike) {
	for (int i = 0; i < bikes.size(); i++)
		if (bikes.at(i) == bike) return i;
	return -1;
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

void goToLivingBike(bool next) {
	int tries = 0, // loop would be infinite if all bikes are dead, but game should have ended then
	    newBikeID = viewedBikeID;;
	do {
		if (tries++ > bikes.size()) return;
		newBikeID += next ? 1 : bikes.size()-1;
		newBikeID %= bikes.size();
	} while (bikes.at(newBikeID)->isDead());
	viewedBikeID = newBikeID;
}

void aiTick(float sec) {
	// TODO control the bots
	// start at 1, because 0 is the player
	for (int i = 1; i < bikes.size(); i++) {
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
			case GLFW_KEY_LEFT:
				if (ownBike->isDying()) goToLivingBike(false);
				else ownBike->turn(false);
				break;
			case GLFW_KEY_RIGHT:
				if (ownBike->isDying()) goToLivingBike(true);
				else ownBike->turn(true);
				break;
		}
	}
}

GLFWwindow *setupWindow() {
	// init GLFW
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) exit(EXIT_FAILURE);

	// open window
	GLFWwindow *window = glfwCreateWindow(800, 600, "Lightbikes", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	glEnable(GL_DEPTH_TEST);

	return window;
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
	GLFWwindow *window = setupWindow();
	while (!glfwGetKey(window, GLFW_KEY_SPACE))
		glfwPollEvents();
	srand(glfwGetTime()*100000);
	newGame();
	while (!glfwWindowShouldClose(window)) {
		static double lastFrameSec = glfwGetTime();
		float frameSec = glfwGetTime() - lastFrameSec;
		lastFrameSec = glfwGetTime();

		///// game processing /////

		aiTick(frameSec);
		for (int i = 0; i < bikes.size(); i++)
			bikes.at(i)->move(frameSec);
		collideAllBikes();
		testForGameOver();

		// TODO increase/decrease speed
		if (glfwGetKey(window, GLFW_KEY_UP)) {}

		static int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		///// drawing /////

		glClearColor(0.5, 0.5, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw first person view

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.7, 100);

		// smooth curves
		// TODO do not turn smoothly when game restarts
		static float yRot = 0;
		const float targetRot = 90*bikes.at(viewedBikeID)->direction;
		float deltaRot = targetRot - yRot;
		while (deltaRot >  180) deltaRot -= 360;
		while (deltaRot < -180) deltaRot += 360;
		yRot += deltaRot/3.0;

		glTranslatef(0, -viewHeight, -7); // a bit from the top
		glRotatef(yRot, 0, 1, 0);
		glTranslatef(-(bikes.at(viewedBikeID)->pos.x),
		             -0,
		             -(bikes.at(viewedBikeID)->pos.z)); // from the position of the viewed bike

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		drawScene();

		// draw map

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,            // left
		        windowWidth,  // right
		        0,            // bottom
		        windowHeight, // top
		        0.1,          // near
		        100);         // far
		glRotatef(90, 1, 0, 0);
		glTranslatef(-0, -2, -windowHeight);
		glScalef(windowWidth / 4 / mapSizeX,
		         1,
		         windowWidth / 4 / mapSizeX);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		drawScene();

		// finish rendering

		glfwSwapBuffers(window);
		glfwPollEvents();

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
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

