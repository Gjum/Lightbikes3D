/* GameInterface.h
 * Provide an interface definition for games. Gets inherited by Game.
 */
#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include "Bike.h"

class GameInterface {
	public:
		GameInterface();
		~GameInterface();

		// controlling game state
		virtual void newGame();
		virtual void closeGame();
		virtual int addController(Controller *controller);
		virtual void removeController(int controllerID);
		virtual int controllersInGame();

		// during game
		virtual bool onFrame(float frameSec);
		virtual void turnBike(int controllerID, bool right);
		virtual Bike *getBike(int controllerID);
		virtual int nextLivingController(int start, bool next);
		virtual bool allControllersDead();
		virtual bool testForGameOver();
		virtual bool collideBikeWithEverything(Bike *bike);

	private:
		std::vector<Bike *> bikes;
		float secondsToNextPhysicsTick;
		bool gameClosed;

		void collideAllBikes();
		void killBike(int controllerID);
		void physicsTick();
};

#endif // GAME_INTERFACE_H

