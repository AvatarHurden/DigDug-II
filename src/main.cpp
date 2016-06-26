/**
Trabalho Final FCG
Arthur Vedana e Vitor Vanacor
*/
#define PI 3.14159265

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <Mmsystem.h>
#include <time.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/gl.h>
#include <gl/glut.h>

#include "glm.h"
#include "bitmap.h"
#include "map.c"
#include "enemy.c"
#include "player.c"
#include "camera.c"

void mainInit();
void initTexture();
void initLight();
void mainRender();
void mainCreateMenu();
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void onWindowReshape(int x, int y);
void mainIdle();
int main(int argc, char **argv);
void setWindow();
void setViewport(GLint left, GLint right, GLint bottom, GLint top);
void updateState();
void renderFloor();
void updateCam();
void newGame();
void displayText( float x, float y, int r, int g, int b, const char *string );

int windowWidth = 900;
int windowHeight = 900;

int windowXPos = 150;
int windowYPos = 100;
double xOffset = -1.9;
double yOffset = -1.3;

int mainWindowId = 0;
int subWindowId = 0;

int i;
bool paused = false;

float backgrundColor[4] = {0.529f,0.807f,0.980f,1.0f};

void initLight() {
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat light_ambient[] = { backgrundColor[0], backgrundColor[1], backgrundColor[2], backgrundColor[3] };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = {4.0, 4.0, 3.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void setViewport(GLint left, GLint right, GLint bottom, GLint top) {
	glViewport(left, bottom, right - left, top - bottom);
}

/**
Initialize
*/
void mainInit() {
	newGame();
}

void newGame(){
    glClearColor(1.0,1.0,1.0,0.0);
	glColor3f(0.0f,0.0f,0.0f);
	setViewport(0, windowWidth, 0, windowHeight);
	glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FOG);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogfv(GL_FOG_COLOR, backgrundColor);
    glFogf(GL_FOG_DENSITY, 0.05);
	initLight();
    newMap();
    newPlayer();
    newEnemies();
    newCamera(windowWidth, windowHeight);
}

void renderScene(bool isMiniMap) {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	CameraUpdate(isMiniMap);
    PlayerDraw();
    EnemyDrawAll();
	MapDraw();
}

/**
Render scene
*/
void mainRender() {
    if (!paused){
        PlayerUpdate();
        EnemyUpdateAll(getPositionXZ(player.x, player.z));
    }
	renderScene(false);
	glFlush();
	glutPostRedisplay();
	Sleep(30);
}

void miniMapRender(){
    glutSetWindow(subWindowId);
    renderScene(true);
    glFlush();
    glutPostRedisplay();
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
	if (key == 'r') newGame();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
    PlayerHandleInput(key, false);
    if (key == 'v') CameraChangeType();
    if (key == 'p') if(paused) paused = false; else paused = true;
    if (key == 27) exit(0);
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	CameraResize(windowWidth, windowHeight);
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
	mainWindowId = glutCreateWindow("Dig Dug II");
    glutDisplayFunc(mainRender);
	glutReshapeFunc(onWindowReshape);
	glutIdleFunc(mainIdle);
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	mainInit();
	subWindowId = glutCreateSubWindow(mainWindowId, 0, 0,windowWidth/3, windowHeight/3);
    glutDisplayFunc(miniMapRender);
    mainInit();

	glutMainLoop();

    return 0;
}
