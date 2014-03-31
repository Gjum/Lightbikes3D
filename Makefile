run: bin/Client
	@echo ===== Running Client =============================
	@bin/Client
	@echo ===== Done =======================================

bin/Client: obj/Client obj/Bike.o obj/Player.o obj/Game.o
	@echo ===== Linking Client =============================
	@mkdir -p bin/
	@g++ -o bin/Client obj/Client obj/Bike.o obj/Player.o obj/Game.o \
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

obj/Player.o: src/Player.h src/Player.cpp src/Settings.h obj/Game.o
	@echo ===== Compiling Player ===========================
	@mkdir -p obj/
	@g++ -o obj/Player.o -c src/Player.cpp -std=c++11
	@echo ===== Player compiled ============================

obj/Game.o: src/Game.h src/Game.cpp src/Settings.h obj/Bike.o
	@echo ===== Compiling Game =============================
	@mkdir -p obj/
	@g++ -o obj/Game.o -c src/Game.cpp -std=c++11
	@echo ===== Game compiled ==============================

clean:
	@rm -rf bin/ obj/
