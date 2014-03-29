#include "Player.h"

#include <SFML/OpenGL.hpp>
#include "Settings.h"

extern void eventCallback(sf::Event);
extern int goToLivingBike(int, bool);
extern void drawScene();

extern std::vector<Bike *> bikes;

Player::Player(int bikeIDArg) {
	sf::ContextSettings cs;
	cs.depthBits = 24;
	window = new sf::Window(sf::VideoMode(650, 500), "Lightcycles", sf::Style::Default, cs);

	if (!window) {
		exit(EXIT_FAILURE);
	}
	window->setFramerateLimit(60);
	window->setActive(true);
	glEnable(GL_DEPTH_TEST);

	bikeID = bikeIDArg;
	viewedBikeID = bikeID;
	controlKeyLeft = sf::Keyboard::Unknown;
	controlKeyRight = sf::Keyboard::Unknown;
	yRot = 0;
}

Player::~Player() {
	delete window;
}

void Player::setControls(sf::Keyboard::Key left, sf::Keyboard::Key right) {
	controlKeyLeft = left;
	controlKeyRight = right;
}

void Player::onNewGame() {
	viewedBikeID = bikeID;
}

void Player::turnBike(bool right) {
	Bike *bike = bikes.at(bikeID);
	if (!bike->isDying()) bike->turn(right);
	else viewedBikeID = goToLivingBike(viewedBikeID, right);
}

void Player::drawWindow() {
	window->setActive(true);

	int windowWidth = window->getSize().x;
	int windowHeight = window->getSize().y;
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

	window->display();
}

