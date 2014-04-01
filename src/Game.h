#ifndef GAME_H
#define GAME_H

#include "Bike.h"

class Game {
	public:
		Game();
		~Game();

		void newGame();
		bool onFrame(float frameSec);
		bool playerWantsToJoin(Controller *controller);
		void closeGame();

		int bikesInGame();
		Bike *getBike(int bikeID);
		int nextLivingBike(int start, bool next);
		bool allBikesDead();
		bool testForGameOver();
		bool collideBikeWithEverything(Bike *bike);

	private:
		std::vector<Bike *> bikes;
		float secondsToNextPhysicsTick;
		bool gameClosed;
		int numNonAiPlayers;

		void collideAllBikes();
		void killBike(int bikeID);
		void physicsTick();
};

#endif // GAME_H

