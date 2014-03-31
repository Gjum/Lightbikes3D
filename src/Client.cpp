#include <SFML/Window.hpp>

#include "Game.h"
#include "Player.h"
#include "Bike.h"
#include "Settings.h"

Game *game;
Player *players[2];
bool clientRunning = true;

void newGame() {
	if (!clientRunning) return;
	game->newGame();
	players[0]->onNewGame();
	players[1]->onNewGame();
}

void aiTick(float sec) {
	// TODO control the bots
	// start at 2, because 0 and 1 are the players
	for (int i = 2; i < game->bikesInGame(); i++) {
		Bike *bike = game->getBike(i);
		if (bike->isDying()) continue;
		Bike *ghostA = new Bike(bike);
		bike->wallHeight = 0; // simulate death to be ignored on collision tests
		bool right, turn = false;
		// move twice to avoid frontal crash with other bike
		ghostA->move(sec);
		ghostA->move(sec);
		if (game->collideBikeWithEverything(ghostA) || ghostA->collideWithWalls(ghostA)) {
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

void pollEvents(sf::Window *window) {
	sf::Event event;
	while (window->pollEvent(event)) {
		switch (event.type) {
			case sf::Event::KeyPressed:
				switch (event.key.code) {
					case sf::Keyboard::Space:
					case sf::Keyboard::Return:
						newGame();
						break;
					case sf::Keyboard::Escape:
					case sf::Keyboard::Q:
						clientRunning = false;
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
				printf("Window closed\n");
				break;
			default:
				break;
		}
	}
}

void updateControls(float frameSec) {
	pollEvents(players[0]->window);
	pollEvents(players[1]->window);
	// TODO increase/decrease bike speeds
	aiTick(frameSec);
}

void updateView(float frameSec) {
	players[0]->drawWindow();
	players[1]->drawWindow();
}

void printFps(float frameSec) {
	static int frames = 0;
	frames++;
	static float fpsAccumulator = 0;
	fpsAccumulator += frameSec;
	if (fpsAccumulator > 1) {
		// a second passed
		fpsAccumulator -= 1;
		printf("%3i fps\n", frames);
		frames = 0;
	}
}

void initPlayers(Game *game) {
	players[0] = new Player(game, 0);
	players[1] = new Player(game, 1);
	players[0]->window->setPosition(sf::Vector2i(0, 0));
	players[1]->window->setPosition(sf::Vector2i(650, 0));
	players[0]->setControls(sf::Keyboard::A, sf::Keyboard::D);
	players[1]->setControls(sf::Keyboard::Left, sf::Keyboard::Right);
}

int main() {
	srand(time(NULL));
	game = new Game();
	initPlayers(game);
	newGame();
	sf::Clock clock;
	while (clientRunning) {
		float frameSec = clock.restart().asMicroseconds() / 1000000.0;
		updateControls(frameSec);
		bool gameOver = game->onFrame(frameSec);
		updateView(frameSec);
		if (gameOver) newGame();
		printFps(frameSec);
	}
	delete players[0];
	delete players[1];
	return EXIT_SUCCESS;
}

