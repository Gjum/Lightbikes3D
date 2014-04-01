run: bin/Client
	@echo ===== Running Client =============================
	@bin/Client
	@echo ===== Done =======================================

bin/Client: obj/Client obj/Bike.o obj/Controller.o obj/Player.o obj/AiController.o obj/Game.o
	@echo ===== Linking Client =============================
	@mkdir -p bin/
	@g++ -o bin/Client obj/Client obj/Bike.o obj/Controller.o obj/Player.o obj/AiController.o obj/Game.o \
		-lsfml-window -lsfml-system -lGL -lGLU -lGLEW
	@echo ===== Client linked ==============================

obj/Client: src/Client.cpp src/Settings.h
	@echo ===== Compiling Client ===========================
	@mkdir -p obj/
	@g++ -o obj/Client -c src/Client.cpp -std=c++11
	@echo ===== Client compiled ============================

obj/Bike.o: src/Bike.h src/Bike.cpp src/Settings.h
	@echo ===== Compiling Bike =============================
	@mkdir -p obj/
	@g++ -o obj/Bike.o -c src/Bike.cpp -std=c++11
	@echo ===== Bike compiled ==============================

obj/Player.o: src/Player.h src/Player.cpp src/Controller.h src/Settings.h src/Game.h
	@echo ===== Compiling Player ===========================
	@mkdir -p obj/
	@g++ -o obj/Player.o -c src/Player.cpp -std=c++11
	@echo ===== Player compiled ============================

obj/AiController.o: src/AiController.h src/AiController.cpp src/Controller.h src/Settings.h src/Bike.h src/Game.h
	@echo ===== Compiling AiController =====================
	@mkdir -p obj/
	@g++ -o obj/AiController.o -c src/AiController.cpp -std=c++11
	@echo ===== AiController compiled ======================

obj/Controller.o: src/Controller.h src/Controller.cpp
	@echo ===== Compiling Controller =======================
	@mkdir -p obj/
	@g++ -o obj/Controller.o -c src/Controller.cpp -std=c++11
	@echo ===== Controller compiled ========================

obj/Game.o: src/Game.h src/Game.cpp src/Settings.h src/Bike.h
	@echo ===== Compiling Game =============================
	@mkdir -p obj/
	@g++ -o obj/Game.o -c src/Game.cpp -std=c++11
	@echo ===== Game compiled ==============================

clean:
	@rm -rf bin/ obj/
