bin/Lightbikes3D: src/Lightbikes3D.cpp
	@echo === Compiling Lightbikes3D =======================
	@mkdir -p bin/
	@g++ -o bin/Lightbikes3D src/Lightbikes3D.cpp -lGL -lGLU -lglfw3 -lXrandr -lXi -lX11 -lXxf86vm -lpthread -std=c++11
	@echo === Lightbikes3D compiled ========================

run: bin/Lightbikes3D
	@echo === Running Lightbikes3D =========================
	@bin/Lightbikes3D
	@echo === Done =========================================

clean:
	rm -rf bin/ obj/

