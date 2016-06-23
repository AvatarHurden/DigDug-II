/**
Trabalho Final FCG
Arthur Vedana e Vitor Vanacor
*/

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/gl.h>
#include <gl/glut.h>

#include "glm.h"

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"
#include "map.c"
#include "player.c"
#include "enemy.c"
#include "camera.c"

#pragma comment(lib, "OpenAL32.lib")
#pragma comment(lib, "alut.lib")

#define PI 3.14159265

// sound stuff
#define NUM_BUFFERS 1
#define NUM_SOURCES 1
#define NUM_ENVIRONMENTS 1

#define SMOOTH 0
#define SMOOTH_MATERIAL 1
#define SMOOTH_MATERIAL_TEXTURE 2

void mainInit();
void initTexture();
void initLight();
void createGLUI();
void mainRender();
void mainCreateMenu();
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onGLUIEvent(int id);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();
void newGame();

/**
Screen dimensions
*/
int windowWidth = 600;
int windowHeight = 480;

/**
Screen position
*/
int windowXPos = 100;
int windowYPos = 150;

int mainWindowId = 0;

double xOffset = -1.9;
double yOffset = -1.3;
int mouseLastX = 0;
int mouseLastY = 0;

int i;                       /* Looping var */

float backgrundColor[4] = {0.529f,0.807f,0.980f,1.0f};

void initLight() {
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position0[] = {4.0, 4.0, 3.0, 1.0 };
	GLfloat light_position1[] = {0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}

/**
Initialize
*/
void mainInit() {
	glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setViewport(0, windowWidth, 0, windowHeight);

	// habilita o z-buffer
	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_FOG);
    //	glFogfv(GL_FOG_COLOR, backgrundColor);
    //	glFogf(GL_FOG_DENSITY, 0.1);
	initLight();
	newGame();
}

void newGame(){
    newMap("../../res/lower.bmp", "../../res/upper.bmp");
    newPlayer();
    newEnemies();
    newCamera(windowWidth, windowHeight);
}

void renderScene() {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CameraUpdate();
    PlayerDraw();
    EnemyDrawAll();

	MapDraw();
}

/**
Render scene
*/
void mainRender() {
	PlayerUpdate();
	EnemyUpdateAll();
	renderScene();
	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

void tentaCavar() {
    if (player.digging)
        return;

    player.digging = true;

    if (getTileXZ(player.x, player.z) != HOLE)
        return;

    int dir = player.roty / 90;

    int zIncrement = 0, xIncrement = 0;
    if (dir == 0)
        zIncrement = -1;
    else if (dir == 1)
        xIncrement = 1;
    else if (dir == 2)
        zIncrement = 1;
    else
        xIncrement = -1;

    Position start = getPositionXZ(player.x, player.z);
    do {
        start.x += xIncrement;
        start.z += zIncrement;
        if (getTile(start.x, start.z) == BLOCK)
            return;
        for (i = 0; i < m.numEnemies; i++)
            if (getPositionXZ(enemies[i].x, enemies[i].z).x == start.x &&
                getPositionXZ(enemies[i].x, enemies[i].z).z == start.z) {
                return;
            }
    } while (getTile(start.x, start.z) != EMPTY);

    start = getPositionXZ(player.x, player.z);
    start.x += xIncrement;
    start.z += zIncrement;
    while (getTile(start.x, start.z) != EMPTY) {
        setTile(start.x, start.z, CRACK);
        start.x += xIncrement;
        start.z += zIncrement;
    }
}

/**
Handles events from the mouse right button menu
*/
void mainHandleMouseRightButtonMenuEvent(int option) {
	switch (option) {
		case 1 :
			exit(0);
			break;
		default:
			break;
	}
}

/**
Create mouse button menu
*/
void mainCreateMenu() {
	glutCreateMenu(mainHandleMouseRightButtonMenuEvent);
	glutAddMenuEntry("Quit", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

/**
Key press event handler
*/
void onKeyDown(unsigned char key, int x, int y) {
	PlayerHandleInput(key, true);
	if (key == ' ') tentaCavar();
	if (key == 'r') newGame();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
    PlayerHandleInput(key, false);
    if (key == 118) CameraChangeType(); //v
    if (key == 27) exit(0);
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	newCamera(windowWidth, windowHeight);
	setViewport(0, windowWidth, 0, windowHeight);
}

void mainIdle() {
	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);
	mainWindowId = glutCreateWindow("FPS");

	glutDisplayFunc(mainRender);
	glutReshapeFunc(onWindowReshape);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

	mainInit();
	glutMainLoop();

    return 0;
}
