#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

const float bikeRadius = .5;
const float wallRadius = .1;
const float defaultBikeSpeed = 7;
const float wallShrinkSpeed = 3;

struct Point {
	float x, z;
};

struct Bike {
	Point pos;
	char direction;
	float wallHeight, speed;
	float color[3];
	std::vector<Point> walls;
};

std::vector<Bike *> bikes;
Bike *ownBike = NULL;
bool viewFromTop = false;

///// game logic /////

bool isBikeDying(Bike *bike) {
	return bike->wallHeight < 1;
}

bool isBikeDead(Bike *bike) {
	return bike->wallHeight <= 0;
}

bool collideBikeWithBike(Bike *bikeA, Bike *bikeB) {
	// TODO collideBikeWithBike
	// TODO print debugging message when colliding bike with itself
	return false;
}

bool collideBikeWithWalls(Bike *bikeA, Bike *bikeB) {
	// TODO variable names... :p
	Point wa, wb;
	float bw = bikeA->pos.x - bikeRadius; // west
	float be = bikeA->pos.x + bikeRadius; // east
	float bn = bikeA->pos.z - bikeRadius; // north
	float bs = bikeA->pos.z + bikeRadius; // south
	// do not collide with the two newest walls if bikeA == bikeB
	if (bikeA == bikeB) {
		if (bikeB->walls.size() < 3) return false;
		wa = bikeB->walls.at(bikeB->walls.size() - 2); // start at third-newest wall
	}
	else wa = bikeB->pos;
	for (int i = bikeB->walls.size() - (bikeA == bikeB ? 3 : 1); i >= 0; i--) {
		wb = bikeB->walls.at(i);
		float ww = ((wa.x < wb.x) ? wa.x : wb.x) - wallRadius; // west
		float we = ((wa.x > wb.x) ? wa.x : wb.x) + wallRadius; // east
		float wn = ((wa.z < wb.z) ? wa.z : wb.z) - wallRadius; // north
		float ws = ((wa.z > wb.z) ? wa.z : wb.z) + wallRadius; // south
		if (ww <= be && we >= bw && wn <= bs && ws >= bn)
			return true;
		wa = wb;
	}
	return false;
}

void resetBikeWalls(Bike *bike) {
	bike->walls.clear();
	bike->walls.push_back(bike->pos);
}

void setBikeColor(Bike *bike, float r, float g, float b) {
	bike->color[0] = r;
	bike->color[1] = g;
	bike->color[2] = b;
}

void deleteBike(Bike *bike) {
	// destructor stuff goes here
	delete bike;
}

void killBike(Bike *bike) {
	if (isBikeDying(bike)) return;
	bike->wallHeight = .99999;
	// TODO end/restart game if only one left
	if (bike == ownBike) viewFromTop = true;
}

void moveBike(Bike *bike, float sec) {
	// do not move dying bikes, make walls smaller instead
	if (isBikeDying(bike)) {
		bike->wallHeight = bike->wallHeight - sec*wallShrinkSpeed;
		if (bike->wallHeight < 0) bike->wallHeight = 0; // prevent overflow
	}
	else {
		switch (bike->direction) {
			case 0:
				bike->pos.z -= sec * bike->speed;
				break;
			case 1:
				bike->pos.x += sec * bike->speed;
				break;
			case 2:
				bike->pos.z += sec * bike->speed;
				break;
			case 3:
				bike->pos.x -= sec * bike->speed;
				break;
		}
	}
}

void turnBike(Bike *bike, bool right) {
	if (isBikeDying(bike)) return;
	bike->direction = (bike->direction + (right ? 1 : 3)) % 4;
	bike->walls.push_back(bike->pos);
}

void collideAllBikes() {
	for (int i = 0; i < bikes.size(); i++) {
		Bike *bikeA = bikes.at(i);
		if (isBikeDead(bikeA)) continue;
		// collide with own wall
		if (collideBikeWithWalls(bikeA, bikeA)) {
			killBike(bikeA);
			continue;
		}
		for (int j = i+1; j < bikes.size(); j++) {
			Bike *bikeB = bikes.at(j);
			if (isBikeDead(bikeB)) continue;
			if (collideBikeWithBike(bikeA, bikeB)) {
				killBike(bikeA);
				killBike(bikeB);
			}
			else {
				if (collideBikeWithWalls(bikeA, bikeB))
					killBike(bikeA);
				if (collideBikeWithWalls(bikeB, bikeA))
					killBike(bikeB);
			}
		}
	}
}

void newGame() {
	for (int i = 0; i < bikes.size(); i++)
		deleteBike(bikes.at(i));
	bikes.clear();

	for (int i = 0; i < 5; i++) {
		Bike *bike = new Bike;
		bikes.push_back(bike);
		// TODO better bike initialization
		bike->pos.x = 2*i;
		bike->pos.z = random()%10;
		bike->direction = 0;
		bike->speed = defaultBikeSpeed;
		bike->wallHeight = 1;
		resetBikeWalls(bike);
		setBikeColor(bike,
				random()%2,
				random()%2,
				random()%2);
	}
	// own bike
	ownBike = bikes.at(0);
	ownBike->pos.z = -15;
	ownBike->direction = 2;
	resetBikeWalls(ownBike);
	viewFromTop = false;
}

///// OpenGL and GLFW /////

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_ENTER:
			case GLFW_KEY_Q:
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			case GLFW_KEY_SPACE:
				newGame();
				break;
			case GLFW_KEY_LEFT:
				turnBike(ownBike, false);
				break;
			case GLFW_KEY_RIGHT:
				turnBike(ownBike, true);
				break;
		}
	}
}

GLFWwindow *setupWindow() {
	// init GLFW
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) exit(EXIT_FAILURE);

	// open window
	GLFWwindow *window = glfwCreateWindow(800, 600, "Lightbikes", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_DEPTH_TEST);

	return window;
}

static void drawBikeAndWalls(Bike *bike) {
	glColor3fv(bike->color);
	// TODO fancy bike
	if (!isBikeDying(bike)) {
		glPushMatrix();
		glTranslatef(bike->pos.x, 0, bike->pos.z);

		glBegin(GL_QUADS);

		glVertex3f( 0.5, 1,  0.5);
		glVertex3f(-0.5, 1,  0.5);
		glVertex3f(-0.5, 1, -0.5);
		glVertex3f( 0.5, 1, -0.5);

		glVertex3f( 0.5, 0,  0.5);
		glVertex3f(-0.5, 0,  0.5);
		glVertex3f(-0.5, 0, -0.5);
		glVertex3f( 0.5, 0, -0.5);

		glEnd();

		glPopMatrix();
	}
	// walls
	if (!isBikeDead(bike)) {
		glBegin(GL_QUADS);

		Point wa = bike->pos, wb;
		for (int i = bike->walls.size() - 1; i >= 0; i--) {
			wb = bike->walls.at(i);
			float ww = ((wa.x < wb.x) ? wa.x : wb.x) - wallRadius; // west
			float we = ((wa.x > wb.x) ? wa.x : wb.x) + wallRadius; // east
			float wn = ((wa.z < wb.z) ? wa.z : wb.z) - wallRadius; // north
			float ws = ((wa.z > wb.z) ? wa.z : wb.z) + wallRadius; // south

			float bh = bike->wallHeight;

			// western wall
			glVertex3f(ww,  0, wn);
			glVertex3f(ww, bh, wn);
			glVertex3f(ww, bh, ws);
			glVertex3f(ww,  0, ws);

			// eastern wall
			glVertex3f(we,  0, wn);
			glVertex3f(we, bh, wn);
			glVertex3f(we, bh, ws);
			glVertex3f(we,  0, ws);

			// northern wall
			glVertex3f(ww,  0, wn);
			glVertex3f(ww, bh, wn);
			glVertex3f(we, bh, wn);
			glVertex3f(we,  0, wn);

			// southern wall
			glVertex3f(ww,  0, ws);
			glVertex3f(ww, bh, ws);
			glVertex3f(we, bh, ws);
			glVertex3f(we,  0, ws);

			// top of the wall
			glVertex3f(ww, bh, wn);
			glVertex3f(ww, bh, ws);
			glVertex3f(we, bh, ws);
			glVertex3f(we, bh, wn);

			wa = wb;
		}

		glEnd();
	}
}

///// main /////

int main() {
	GLFWwindow *window = setupWindow();
	while (!glfwGetKey(window, GLFW_KEY_SPACE))
		glfwPollEvents();
	srand(glfwGetTime()*100000);
	newGame();
	while (!glfwWindowShouldClose(window)) {
		static double lastFrameSec = glfwGetTime();
		float frameSec = glfwGetTime() - lastFrameSec;
		lastFrameSec = glfwGetTime();

		for (int i = 0; i < bikes.size(); i++)
			moveBike(bikes.at(i), frameSec);

		collideAllBikes();

		// TODO increase/decrease speed
		if (glfwGetKey(window, GLFW_KEY_UP)) {}

		static int windowWidth, windowHeight;
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		glClearColor(0.5, 0.5, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set projection perspective
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.7, 100);
		if (viewFromTop) {
			glRotatef(90, 1, 0, 0);
			glTranslatef(-(ownBike->pos.x), -30, -(ownBike->pos.z)); // from the own bike's position
		}
		else {
			// smooth curves
			// TODO do not turn smoothly when game restarts
			static float yRot = 0;
			const float targetRot = 90*ownBike->direction;
			float deltaRot = targetRot - yRot;
			while (deltaRot >  180) deltaRot -= 360;
			while (deltaRot < -180) deltaRot += 360;
			yRot += deltaRot/3.0;
			glTranslatef(0, -4, -7); // a bit from the top
			glRotatef(yRot, 0, 1, 0);
			glTranslatef(-(ownBike->pos.x), 0, -(ownBike->pos.z)); // from the own bike's position
		}

		// render objects
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// draw scene
		for (int i = 0; i < bikes.size(); i++)
			drawBikeAndWalls(bikes.at(i));

		// fps calculations
		static int frames = 0;
		frames++;
		static double lastSecond = glfwGetTime();
		if (glfwGetTime() - lastSecond > 1) {
			// a second passed
			lastSecond++;
			printf("%3i fps\n", frames);
			frames = 0;
		}

		// finish rendering
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

