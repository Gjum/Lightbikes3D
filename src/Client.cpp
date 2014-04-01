#include <SFML/Window.hpp>

#include "Game.h"
#include "Player.h"

void initPlayers(Game *game, Player **players) {
	players[0] = new Player(game, 0, players);
	players[1] = new Player(game, 1, players);
	players[0]->window->setPosition(sf::Vector2i(0, 0));
	players[1]->window->setPosition(sf::Vector2i(650, 0));
	players[0]->setControls(sf::Keyboard::A, sf::Keyboard::D);
	players[1]->setControls(sf::Keyboard::Left, sf::Keyboard::Right);
	if (!game->playerWantsToJoin((Controller *) players[0]))
		delete players[0];
	if (!game->playerWantsToJoin((Controller *) players[1]))
		delete players[1];
}

int main() {
	Game *game = new Game();
	Player *players[2];
	initPlayers(game, players);
	game->newGame();
	sf::Clock clock;
	while (true) {
		float frameSec = clock.restart().asMicroseconds() / 1000000.0;
		bool gameClosed = game->onFrame(frameSec);
		if (gameClosed) break;
	}
	delete game;
	return EXIT_SUCCESS;
}

