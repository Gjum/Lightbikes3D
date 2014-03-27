run: bin/Lightbikes3D
	@echo ===== Running Lightbikes3D =======================
	@bin/Lightbikes3D
	@echo ===== Done =======================================

bin/Lightbikes3D: src/Lightbikes3D.cpp bin/Bike.o
	@echo ===== Compiling Lightbikes3D =====================
	@mkdir -p bin/
	@g++ -o bin/Lightbikes3D src/Lightbikes3D.cpp bin/Bike.o -lGL -lGLU -lglfw3 -lXrandr -lXi -lX11 -lXxf86vm -lpthread -std=c++11
	@echo ===== Lightbikes3D compiled ======================

bin/Bike.o: src/Bike.h src/Bike.cpp
	@echo ===== Compiling Bike =============================
	@mkdir -p bin/
	@g++ -o bin/Bike.o -c src/Bike.cpp -std=c++11
	@echo ===== Bike compiled ==============================

clean:
	rm -rf bin/ obj/

