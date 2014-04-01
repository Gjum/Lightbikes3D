#ifndef AI_CONTROLLER_H
#define AI_CONTROLLER_H

#include "Bike.h"
#include "Game.h"
#include "Controller.h"

class AiController: public Controller {
	public:
		AiController(Game *game, int bikeID);
		~AiController();

		void onNewGame();
		void updateControls();
		void updateView(float frameSec);

	private:
		Game *game;
		Bike *bike;
		int bikeID;

		bool canGoForward();
		bool canTurn(bool right);
		bool preferredTurnSide();
};

#endif // AI_CONTROLLER_H

