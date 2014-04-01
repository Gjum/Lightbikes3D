#include <SFML/Window.hpp>

#include "Game.h"
#include "Player.h"
#include "AiController.h"

void initPlayers(Game *game, Player **players) {
	players[0] = new Player(game, 0, players);
	players[1] = new Player(game, 1, players);
	players[0]->window->setPosition(sf::Vector2i(0, 0));
	players[1]->window->setPosition(sf::Vector2i(650, 0));
	players[0]->setControls(sf::Keyboard::A, sf::Keyboard::D);
	players[1]->setControls(sf::Keyboard::Left, sf::Keyboard::Right);
	game->addController((Controller *) players[0]);
	game->addController((Controller *) players[1]);
	for (int i = 2; i < 6; i++)
		game->addController((Controller *) new AiController(game, i));
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
	printf("Game closed. See you soon!\n");
	delete game;
	return EXIT_SUCCESS;
}

