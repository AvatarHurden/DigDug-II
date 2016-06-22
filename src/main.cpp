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
#include "player.c"
#include "camera.c"
#include "enemy.c"
#include "map.c"

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

Player player;
Camera camera;
Enemy* enemies;
Map m;

// Aux function to load the object using GLM and apply some functions
bool C3DObject_Load_New(const char *pszFilename, GLMmodel **model)
{
    char aszFilename[256];
    strcpy(aszFilename, pszFilename);

    if (*model) {
        free(*model);
        *model = NULL;
    }

    *model = glmReadOBJ(aszFilename);
    if (!(*model))
    return false;

    glmUnitize(*model);
    glmScale(*model,0.12); // USED TO SCALE THE OBJECT
    glmFacetNormals(*model);
    glmVertexNormals(*model, 90.0);

    return true;
}

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

	m = newMap("../../res/lower.bmp", "../../res/upper.bmp");

	enemies = (Enemy*) malloc(sizeof(Enemy)*m.numEnemies);
	for (i = 0; i < m.numEnemies; i++)
        enemies[i] = newEnemy();

    player = newPlayer();
    camera = newCamera(&player, windowWidth, windowHeight);

	setPlayerPosition(m, &player);
	setEnemyPositions(m, enemies);

	initLight();
}

void renderScene() {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CameraUpdate(camera);
    PlayerDraw(player);
    for (i = 0; i < m.numEnemies; i++)
        EnemyDraw(enemies[i]);

    // binds the bmp file already loaded to the OpenGL parameters
    //glBindTexture(GL_TEXTURE_2D, texture);

	//renderFloor();
	MapDraw(m);
}

/**
Render scene
*/
void mainRender() {
	PlayerUpdate(&player);
    for (i = 0; i < m.numEnemies; i++)
        EnemyUpdate(&enemies[i], player);
	renderScene();
	glFlush();
	glutPostRedisplay();
	Sleep(30);
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
	//printf("%d \n", key);
	switch (key) {
		case 119: //w
			player.goingForward = true;
			break;
		case 115: //s
			player.goingBackward = true;
			break;
		case 97: //a
            if (!player.turningRight)
                player.turningLeft = true;
			break;
		case 100: //d
		    if (!player.turningLeft)
                player.turningRight = true;
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

/**
Key release event handler
*/
void onKeyUp(unsigned char key, int x, int y) {
	switch (key) {
		case 119: //w
			player.goingForward = false;
			break;
		case 115: //s
			player.goingBackward = false;
			break;
        case 118: //v
            CameraChangeType(&camera);
            break;
		case 27:
			exit(0);
			break;
		default:
			break;
	}

	//glutPostRedisplay();
}

void onWindowReshape(int x, int y) {
	windowWidth = x;
	windowHeight = y;
	camera = newCamera(&player, windowWidth, windowHeight);
	setViewport(0, windowWidth, 0, windowHeight);
}

/**
Glut idle funtion
*/
void mainIdle() {
	/**
	Set the active window before send an glutPostRedisplay call
	so it wont be accidently sent to the glui window
	*/

	glutSetWindow(mainWindowId);
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(windowWidth,windowHeight);
	glutInitWindowPosition(windowXPos,windowYPos);

	/**
	Store main window id so that glui can send it redisplay events
	*/
	mainWindowId = glutCreateWindow("FPS");

	glutDisplayFunc(mainRender);

	glutReshapeFunc(onWindowReshape);

	/**
	Register keyboard events handlers
	*/
	glutKeyboardFunc(onKeyDown);
	glutKeyboardUpFunc(onKeyUp);

//	glEnable(GL_FOG);
//	glFogfv(GL_FOG_COLOR, backgrundColor);
//	glFogf(GL_FOG_DENSITY, 0.1);

	mainInit();

	glutMainLoop();

    return 0;
}
