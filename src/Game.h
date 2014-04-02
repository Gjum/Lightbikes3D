/* Game.h
 * Holds all bikes and advances them every tick as the game runs. Also handles game start/end, player join/disconnect etc.
 */
#ifndef GAME_H
#define GAME_H

#include "Bike.h"
#include "GameInterface.h"

class Game: public GameInterface {
	public:
		Game();
		~Game();

		// controlling game state
		void newGame();
		void closeGame();
		int addController(Controller *controller);
		void removeController(int controllerID);
		int controllersInGame();

		// during game
		bool onFrame(float frameSec);
		void turnBike(int controllerID, bool right);
		Bike *getBike(int controllerID);
		int nextLivingController(int start, bool next);
		bool allControllersDead();
		bool testForGameOver();
		bool collideBikeWithEverything(Bike *bike);

	private:
		std::vector<Bike *> bikes;
		float secondsToNextPhysicsTick;
		bool gameClosed;

		void collideAllBikes();
		void killBike(int controllerID);
		void physicsTick();
};

#endif // GAME_H

