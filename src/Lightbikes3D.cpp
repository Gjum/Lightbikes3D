#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

const float viewHeight = 1.1;
bool viewFromTop = false;

struct Point {
	float x, z;
};

Point point(float x, float z) {
	Point p;
	p.x = x;
	p.z = z;
	return p;
}

struct Bike {
	Point pos;
	char direction;
	float height, speed;
	float color[3];
	std::vector<Point> walls;
};

std::vector<Bike *> bikes;

///// game logic /////

bool isBikeDying(Bike *bike) {
	return bike->height < 1;
}

bool isBikeDead(Bike *bike) {
	return bike->height <= 0.0001;
}

bool collideBikeWithBike(Bike *bikeA, Bike *bikeB) {
	// TODO collideBikeWithBike
	return false;
}

bool collideBikeWithWalls(Bike *bikeA, Bike *bikeB) {
	// TODO variable names... :p
	// TODO wrong maths... :p
	Point wa = bikeB->pos, wb;
	float bL = bikeA->pos.x - .5; // left
	float bR = bikeA->pos.x + .5; // right
	float bT = bikeA->pos.z - .5; // top
	float bB = bikeA->pos.z + .5; // bottom
	for (int i = bikeB->walls.size() - 1; i >= 0; i--) {
		wb = bikeB->walls.at(i);
		float wL = (wa.x < wb.x) ? wa.x : wb.x; // left
		float wR = (wa.x > wb.x) ? wa.x : wb.x; // right
		float wT = (wa.z < wb.z) ? wa.z : wb.z; // top
		float wB = (wa.z > wb.z) ? wa.z : wb.z; // bottom
		if (wL < bR
				&& wR > bL
				&& wT < bB
				&& wB > bT)
			return true;
		wa = wb;
	}
	return false;
}

void setBikeColor(Bike *bike, float r, float g, float b) {
	bike->color[0] = r;
	bike->color[1] = g;
	bike->color[2] = b;
}

void resetBike(Bike *bike, float x, float z) {
	bike->pos.x = x;
	bike->pos.z = z;
	bike->direction = 0;
	bike->speed = 7;
	bike->height = 1;
	setBikeColor(bike, 1, 0, 1);
	bike->walls.clear();
	bike->walls.push_back(point(x, z));
}

void deleteBike(Bike *bike) {
	// destructor stuff goes here
	delete bike;
}

void killBike(Bike *bike) {
	bike->height = .99999;
	// TODO end/restart game if only one left
	if (bike == bikes.at(0)) viewFromTop = true;
}

void moveBike(Bike *bike, float sec) {
	// do not move dying bikes, make walls smaller instead
	if (isBikeDying(bike) && !isBikeDead(bike)) {
		bike->height -= sec;
		if (bike->height < 0) bike->height = 0;
		return;
	}
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

void turnBike(Bike *bike, bool right) {
	if (isBikeDying(bike)) return;
	bike->direction = (bike->direction + (right ? 1 : 3)) % 4;
	bike->walls.push_back(bike->pos);
}

void collideAllBikes() {
	for (int i = 0; i < bikes.size(); i++) {
		Bike *bikeA = bikes.at(i);
		if (isBikeDead(bikeA)) continue;
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
		resetBike(bike, 2*i, random()%10); // TODO better bike starting pos, color
		setBikeColor(bike,
				(random()%10) / 10.0,
				(random()%10) / 10.0,
				(random()%10) / 10.0);
	}
	// own bike
	resetBike(bikes.at(0), 0, -15);
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
				turnBike(bikes.at(0), false);
				break;
			case GLFW_KEY_RIGHT:
				turnBike(bikes.at(0), true);
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
	// TODO fancy bike
	glColor3fv(bike->color);
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

	// walls
	glBegin(GL_QUADS);
		Point wa = bike->pos, wb;
		for (int i = bike->walls.size() - 1; i >= 0; i--) {
			wb = bike->walls.at(i);
			glVertex3f(wa.x,  0.0, wa.z);
			glVertex3f(wa.x,  1.0, wa.z);
			glVertex3f(wb.x,  1.0, wb.z);
			glVertex3f(wb.x,  0.0, wb.z);
			wa = wb;
		}
	glEnd();
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
			gluLookAt(4, 30,  4,  // eye
			          5, -5,  0,  // center
			          0,  0, -1); // up
		}
		else {
			// smooth curves
			// TODO do not turn smoothly when game restarts
			static float yRot = 0;
			const float targetRot = 90*bikes.at(0)->direction;
			float deltaRot = targetRot - yRot;
			while (deltaRot >  180) deltaRot -= 360;
			while (deltaRot < -180) deltaRot += 360;
			yRot += deltaRot/3.0;
			glRotatef(yRot, 0, 1, 0);
			glTranslatef(-(bikes.at(0)->pos.x), -viewHeight, -(bikes.at(0)->pos.z));
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

