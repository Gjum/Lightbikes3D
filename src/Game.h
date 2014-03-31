#ifndef GAME_H
#define GAME_H

#include "Bike.h"

class Game {
	public:
		Game();
		~Game();

		void newGame();
		bool onFrame(float frameSec);
		int bikesInGame();
		Bike *getBike(int bikeID);
		int nextLivingBike(int start, bool next);
		bool collideBikeWithEverything(Bike *bike);

	private:
		std::vector<Bike *> bikes;

		void collideAllBikes();
		void killBike(int bikeID);
		bool testForGameOver();
};

#endif // GAME_H

