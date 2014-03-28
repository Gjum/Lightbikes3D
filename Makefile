run: bin/Lightcycles3D
	@echo ===== Running Lightcycles3D ======================
	@bin/Lightcycles3D
	@echo ===== Done =======================================

bin/Lightcycles3D: src/Lightcycles3D.cpp obj/Bike.o obj/Player.o
	@echo ===== Compiling Lightcycles3D ====================
	@mkdir -p bin/
	@g++ -o bin/Lightcycles3D src/Lightcycles3D.cpp obj/Bike.o obj/Player.o \
		-lGL -lGLU -lglfw3 -lXrandr -lXi -lX11 -lXxf86vm -lpthread -std=c++11
	@echo ===== Lightcycles3D compiled =====================

obj/Bike.o: src/Bike.h src/Bike.cpp
	@echo ===== Compiling Bike =============================
	@mkdir -p obj/
	@g++ -o obj/Bike.o -c src/Bike.cpp -std=c++11
	@echo ===== Bike compiled ==============================

obj/Player.o: src/Player.h src/Player.cpp
	@echo ===== Compiling Player ===========================
	@mkdir -p obj/
	@g++ -o obj/Player.o -c src/Player.cpp -std=c++11
	@echo ===== Player compiled ============================

clean:
	@rm -rf bin/ obj/

