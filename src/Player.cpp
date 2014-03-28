#include "Player.h"

#include <GL/glu.h>
#include <cstdlib>
#include <vector>

#include "Settings.h"

extern void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern int goToLivingBike(int, bool);
extern void drawScene();

extern std::vector<Bike *> bikes;

Player::Player(int bikeID) {
	window = glfwCreateWindow(650, 500, "Lightcycles", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, keyCallback);
	glfwMakeContextCurrent(window);
	glEnable(GL_DEPTH_TEST);

	viewedBikeID = bikeID;
	bike = bikes.at(viewedBikeID);
	yRot = 0;
}

Player::~Player() {
	glfwDestroyWindow(window);
}

void Player::turnBike(bool right) {
	if (right) {
		if (bike->isDying())
			viewedBikeID = goToLivingBike(viewedBikeID, true);
		else bike->turn(true);
	}
	else {
		if (bike->isDying())
			viewedBikeID = goToLivingBike(viewedBikeID, false);
		else bike->turn(false);
	}
}

void Player::drawWindow() {
	glfwMakeContextCurrent(window);

	static int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.5, 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///// draw first person view /////

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.7, 100);

	// smooth curves
	// TODO do not turn smoothly when game restarts
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

	///// draw map /////

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

	///// finish rendering /////

	glfwSwapBuffers(window);
}

