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

void updateControls() {
	pollEvents(players[0]->window);
	pollEvents(players[1]->window);
}

void updateView() {
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
		updateControls();
		bool gameOver = game->onFrame(frameSec);
		if (gameOver) newGame();
		updateView();
		printFps(frameSec);
	}
	delete players[0];
	delete players[1];
	return EXIT_SUCCESS;
}

