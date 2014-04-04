#include "Player.h"

#include <SFML/OpenGL.hpp>
#include "SettingsGame.h"

Player::Player(GameInterface *game, int controllerID, Player **players) {
	this->game = game;
	this->controllerID = controllerID;
	this->players = players;

	sf::ContextSettings cs;
	cs.depthBits = 24;
	window = new sf::Window(sf::VideoMode(650, 500), "Lightcycles", sf::Style::Default, cs);

	if (!window) exit(EXIT_FAILURE);
	window->setFramerateLimit(60);
	window->setActive(true);
	glEnable(GL_DEPTH_TEST);

	viewedBikeID = controllerID;
	controlKeyLeft = sf::Keyboard::Unknown;
	controlKeyRight = sf::Keyboard::Unknown;
	yRot = 0;
}

Player::~Player() {
	window->close();
	delete window;
}

void Player::setControls(sf::Keyboard::Key left, sf::Keyboard::Key right) {
	controlKeyLeft = left;
	controlKeyRight = right;
}

void Player::onNewGame() {
	viewedBikeID = controllerID;
}

bool Player::canTurn(bool right) {
	Bike *bike = game->getBike(controllerID);
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	ghost->turn(right);
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(game->collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

bool Player::canGoForward() {
	Bike *bike = game->getBike(controllerID);
	if (bike->isDying()) return false;
	Bike *ghost = new Bike(bike);
	bike->wallHeight = 0; // simulate death to be ignored on collision tests
	ghost->onPhysicsTick();
	ghost->onPhysicsTick();
	bool ret = !(game->collideBikeWithEverything(ghost) || ghost->collideWithWalls(ghost));
	delete ghost;
	bike->wallHeight = 1;
	return ret;
}

void Player::turnBike(bool right) {
	Bike *bike = game->getBike(controllerID);
	if (bike->isDying())
		viewedBikeID = game->nextLivingController(viewedBikeID, right);
	else if (canTurn(right)) {
		// prevent impossible turns
		game->turnBike(controllerID, right);
		turnedThisTick = true;
	}
}

void Player::updateControls() {
	turnedThisTick = false;
	sf::Event event;
	while (window->pollEvent(event)) {
		switch (event.type) {
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Space:
					case sf::Keyboard::Return:
						game->newGame();
						break;
					case sf::Keyboard::Escape:
					case sf::Keyboard::Q:
						game->closeGame();
						break;
					default:
						if (event.key.code == players[0]->controlKeyLeft)
							players[0]->turnBike(false);
						else if (event.key.code == players[0]->controlKeyRight)
							players[0]->turnBike(true);
						else if (event.key.code == players[1]->controlKeyLeft)
							players[1]->turnBike(false);
						else if (event.key.code == players[1]->controlKeyRight)
							players[1]->turnBike(true);
						break;
				}
				break;
			case sf::Event::Closed:
				game->closeGame();
				break;
			default:
				break;
		}
	}
	// prevent frontal crashing
	if (!game->getBike(controllerID)->isDying() && !turnedThisTick && !canGoForward()) {
		turnBike(canTurn(true));
	}
}

void Player::drawBikeAndWalls(Bike *bike) {
	// TODO fancy bike
	if (!bike->isDying()) {
		glColor3fv(bike->color);
		glPushMatrix();
		glTranslatef(bike->pos.x, 0, bike->pos.z);

		glBegin(GL_QUADS);

		glVertex3f( bikeRadius, 1,  bikeRadius);
		glVertex3f(-bikeRadius, 1,  bikeRadius);
		glVertex3f(-bikeRadius, 1, -bikeRadius);
		glVertex3f( bikeRadius, 1, -bikeRadius);

		glVertex3f( bikeRadius, 0.001,  bikeRadius);
		glVertex3f(-bikeRadius, 0.001,  bikeRadius);
		glVertex3f(-bikeRadius, 0.001, -bikeRadius);
		glVertex3f( bikeRadius, 0.001, -bikeRadius);

		glEnd();

		glPopMatrix();
	}
	// walls
	if (!bike->isDead()) {
		float h = bike->wallHeight;
		glColor3f(bike->color[0] + (1-h),
		          bike->color[1] + (1-h),
		          bike->color[2] + (1-h));

		glBegin(GL_QUADS);

		for (int i = bike->walls.size() - 1; i >= 0; i--) {
			Box wb = bike->getWallBox(i);

			// wall sides
			// western wall
			glVertex3f(wb.w, 0, wb.n);
			glVertex3f(wb.w, h, wb.n);
			glVertex3f(wb.w, h, wb.s);
			glVertex3f(wb.w, 0, wb.s);
			// eastern wall
			glVertex3f(wb.e, 0, wb.n);
			glVertex3f(wb.e, h, wb.n);
			glVertex3f(wb.e, h, wb.s);
			glVertex3f(wb.e, 0, wb.s);
			// northern wall
			glVertex3f(wb.w, 0, wb.n);
			glVertex3f(wb.w, h, wb.n);
			glVertex3f(wb.e, h, wb.n);
			glVertex3f(wb.e, 0, wb.n);
			// southern wall
			glVertex3f(wb.w, 0, wb.s);
			glVertex3f(wb.w, h, wb.s);
			glVertex3f(wb.e, h, wb.s);
			glVertex3f(wb.e, 0, wb.s);
			// top of the wall
			glVertex3f(wb.w, h, wb.n);
			glVertex3f(wb.w, h, wb.s);
			glVertex3f(wb.e, h, wb.s);
			glVertex3f(wb.e, h, wb.n);
		}

		glEnd();
	}
}

void Player::drawFloorAndBorders() {
	glPushMatrix();
	glBegin(GL_QUADS);

	// floor
	glColor3f(0.2, 0.2, 0.2);

	glVertex3f(       0, -0.0001, 0);
	glVertex3f(       0, -0.0001, mapSizeZ);
	glVertex3f(mapSizeX, -0.0001, mapSizeZ);
	glVertex3f(mapSizeX, -0.0001, 0);

	// grid
	float w = wallRadius*2;
	glColor3f(0.3, 0.3, 0.3);

	for (int i = 0; i < mapSizeX; i += 2) {
		glVertex3f(  i, 0, 0);
		glVertex3f(w+i, 0, 0);
		glVertex3f(w+i, 0, mapSizeZ);
		glVertex3f(  i, 0, mapSizeZ);
	}

	for (int i = 0; i < mapSizeZ; i += 2) {
		glVertex3f(       0, 0,   i);
		glVertex3f(       0, 0, w+i);
		glVertex3f(mapSizeX, 0, w+i);
		glVertex3f(mapSizeX, 0,   i);
	}

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

void Player::drawScene() {
	for (int i = 0; i < game->controllersInGame(); i++)
		drawBikeAndWalls(game->getBike(i));
	drawFloorAndBorders();
}

void Player::updateView(float frameSec) {
	window->setActive(true);

	Bike *viewedBike = game->getBike(viewedBikeID);
	if (viewedBike->isDead()) {
		viewedBikeID = game->nextLivingController(viewedBikeID, true);
		viewedBike = game->getBike(viewedBikeID);
	}

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
	const float targetRot = 90*viewedBike->direction;
	float deltaRot = targetRot - yRot;
	while (deltaRot >  180) deltaRot -= 360;
	while (deltaRot < -180) deltaRot += 360;
	yRot += deltaRot/3.0;

	glTranslatef(0, -3, -8);
	glRotatef(40, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glTranslatef(-(viewedBike->pos.x),
	             -0,
	             -(viewedBike->pos.z)); // from the position of the viewed bike

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
	glRotatef(90, 1, 0, 0); // look down
	glTranslatef(-0, -2, -windowHeight); // make visible and move to the top left
	glScalef(.5/wallRadius, 1, .5/wallRadius); // one pixel per wall width

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawScene();

	///// finish rendering /////

	window->display();
}

