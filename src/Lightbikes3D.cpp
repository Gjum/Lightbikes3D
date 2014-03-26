#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <vector>

const float mapWidth  = 20;
const float mapHeight = 20;
const float bikeRadius = .5;
const float wallRadius = .1;
const float defaultBikeSpeed = 7;
const float wallShrinkSpeed = 3;
const float viewHeight = 4;

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

int getBikeID(Bike *bike) {
	for (int i = 0; i < bikes.size(); i++)
		if (bikes.at(i) == bike) return i;
	return -1;
}

bool isBikeDying(Bike *bike) {
	return bike->wallHeight < 1;
}

bool isBikeDead(Bike *bike) {
	return bike->wallHeight <= 0;
}

bool collideBikeWithBike(Bike *bike, Bike *otherBike) {
	// TODO collideBikeWithBike
	// TODO print debugging message when colliding bike with itself
	return false;
}

bool collideBikeWithWalls(Bike *bike, Bike *otherBike) {
	Point wa, wb;
	float bw = bike->pos.x - bikeRadius; // west
	float be = bike->pos.x + bikeRadius; // east
	float bn = bike->pos.z - bikeRadius; // north
	float bs = bike->pos.z + bikeRadius; // south
	// do not collide with the two newest walls if bike == otherBike
	if (bike == otherBike) {
		if (otherBike->walls.size() < 3) return false;
		wa = otherBike->walls.at(otherBike->walls.size() - 2); // start at third-newest wall
	}
	else wa = otherBike->pos;
	for (int i = otherBike->walls.size() - (bike == otherBike ? 3 : 1); i >= 0; i--) {
		wb = otherBike->walls.at(i);
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

bool collideBikeWithMapBorder(Bike *bike) {
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
	printf("Bike %i crashed\n", getBikeID(bike));
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
		Bike *bike = bikes.at(i);
		if (isBikeDying(bike)) continue;
		// collide with own wall
		if (collideBikeWithWalls(bike, bike)) {
			killBike(bike);
			continue;
		}
		for (int j = i+1; j < bikes.size(); j++) {
			Bike *otherBike = bikes.at(j);
			if (isBikeDying(otherBike)) continue;
			if (collideBikeWithBike(bike, otherBike)) {
				killBike(bike);
				killBike(otherBike);
			}
			else {
				if (collideBikeWithWalls(bike, otherBike))
					killBike(bike);
				if (collideBikeWithWalls(otherBike, bike))
					killBike(otherBike);
			}
		}
		if (collideBikeWithMapBorder(bike)) {
			killBike(bike);
			continue;
		}
	}
}

void newGame() {
	for (int i = 0; i < bikes.size(); i++)
		deleteBike(bikes.at(i));
	bikes.clear();

	static const int bikesNum = 5;
	for (int i = 0; i < bikesNum; i++) {
		Bike *bike = new Bike;
		bikes.push_back(bike);
		// TODO better bike initialization
		bike->pos.x = mapWidth * (i+1) / (bikesNum+1);
		bike->pos.z = 0;
		bike->direction = 2;
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
	ownBike->pos.z = mapHeight;
	ownBike->direction = 0;
	resetBikeWalls(ownBike);
	viewFromTop = false;
}

///// OpenGL and GLFW /////

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

void drawBikeAndWalls(Bike *bike) {
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

void drawFloorAndBorders() {
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);

	// floor
	glVertex3f(       0, -0.01,         0);
	glVertex3f(       0, -0.01, mapHeight);
	glVertex3f(mapWidth, -0.01, mapHeight);
	glVertex3f(mapWidth, -0.01,         0);

	// TODO borders

	glEnd();
	glPopMatrix();
}

void drawScene() {
	for (int i = 0; i < bikes.size(); i++)
		drawBikeAndWalls(bikes.at(i));
	drawFloorAndBorders();
}

///// main /////

int main() {
	GLFWwindow *window = setupWindow();
	while (!glfwGetKey(window, GLFW_KEY_SPACE))
		glfwPollEvents();
	srand(glfwGetTime()*100000);
	newGame();
	while (!glfwWindowShouldClose(window)) {
		///// game processing /////

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

		///// drawing /////

		glClearColor(0.5, 0.5, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw first person view

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.7, 100);

		// smooth curves
		// TODO do not turn smoothly when game restarts
		static float yRot = 0;
		const float targetRot = 90*ownBike->direction;
		float deltaRot = targetRot - yRot;
		while (deltaRot >  180) deltaRot -= 360;
		while (deltaRot < -180) deltaRot += 360;
		yRot += deltaRot/3.0;

		glTranslatef(0, -viewHeight, -7); // a bit from the top
		glRotatef(yRot, 0, 1, 0);
		glTranslatef(-(ownBike->pos.x), 0, -(ownBike->pos.z)); // from the own bike's position

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		drawScene();

		// draw map

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,            // left
				windowWidth,  // right
				0,            // bottom
				windowHeight, // top
				0.1,          // near
				100);         // far
		glRotatef(90, 1, 0, 0);
		glTranslatef(-0, -2, -windowHeight);
		glScalef(windowWidth / 4 / mapWidth,
				 1,
				 windowWidth / 4 / mapWidth);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		drawScene();

		// finish rendering

		glfwSwapBuffers(window);
		glfwPollEvents();

		///// fps calculations /////

		static int frames = 0;
		frames++;
		static double lastSecond = glfwGetTime();
		if (glfwGetTime() - lastSecond > 1) {
			// a second passed
			lastSecond++;
			printf("%3i fps\n", frames);
			frames = 0;
		}
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}

