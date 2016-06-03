/**
Laboratorio FCG - Pratica 4
Ultima modificacao: 09/05/2016 - Acompanha bibliotecas: bitmap.c(.h), glm.cpp,
glmimg.cpp, Texture.cpp(Texture.h)

Implementa a movimentação simples de uma camera em primeira pessoa para um personagem que anda
sobre um plano. Utiliza a biblioteca bitmap para carregamento simples de texturas, glm
para modelos OBJ e glmimg + Texture para aplicacao de texturas TGA sobre os modelos carregados

Tarefas:

1 - Adicionar neblina à cena ao implementar a função enableFog() com os parâmetros de habilitação de fog em OpenGL;
2 - Usar uma imagem no formato BMP como mapa pra posicionar objetos 3D na cena. Use a cor de cada pixel para definir
qual modelo será colocado e a posição do pixel para definir a posição do modelo no espaço. Pixels pretos não representam
nenhum modelo, fazendo com que a posição fique vazia na cena;
3 - Mudar as configurações da fonte de luz da cena, já implementada no código base dado, para que
seja uma spotlight;
4 - Adicionar uma segunda fonte de luz que fique girando ao redor do cenário.
*/

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <iostream>
#include <gl/glut.h>

#include "glm.h"

//openal (sound lib)
#include <al/alut.h>

//bitmap class to load bitmaps for textures
#include "bitmap.h"

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
void initSound();
void initTexture();
void initModel();
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

bool upPressed = false;
bool downPressed = false;

float lightX = 0;
float lightY = 0;
float lightZ = 0;

float planeSize = 8.0f;

// parte de código extraído de "texture.c" por Michael Sweet (OpenGL SuperBible)
// texture buffers and stuff
int i;                       /* Looping var */
BITMAPINFO	*info;           /* Bitmap information */
GLubyte	    *bits;           /* Bitmap RGB pixels */
GLubyte     *ptr;            /* Pointer into bit buffer */
GLubyte	    *rgba;           /* RGBA pixel buffer */
GLubyte	    *rgbaptr;        /* Pointer into RGBA buffer */
GLubyte     temp;            /* Swapping variable */
GLenum      type;            /* Texture type */
GLuint      texture;         /* Texture object */

float backgrundColor[4] = {0.0f,0.0f,0.0f,1.0f};

int totalObjects = 0;
struct object
{
    GLMmodel *model;
    int x;
    int z;
} objects[500];

/*************** PLAYER ********************************************************/
typedef struct PLAYER{
    GLMmodel *model;
    float x;
    float y;
    float z;
    float speedX;
    float speedY;
    float speedZ;
    int roty;
    float rotx;
    bool turningLeft;
    bool turningRight;
    float headPosAux;
} Player;

Player newPlayer(){
    printf("New Player.\n");
    Player p;
    p.x = 0.0f;
    p.y = 0.0f;
    p.z = 2.0f;
    p.speedX = 0.0f;
    p.speedY = 0.0f;
    p.speedZ = 0.0f;
    p.roty = 0;
    p.rotx = 90.0f;
    p.turningLeft = false;
    p.turningRight = false;
    p.headPosAux = 0.0f;
    return p;
}

void PlayerUpdate(Player* p) {
    //printf("Player Update\n");
    if ((*p).turningRight || (*p).turningLeft) {
        if ((*p).turningRight) {
            (*p).roty += 10;
            if ((*p).roty % 90 == 0)
                (*p).turningRight = false;
        } else if ((*p).turningLeft) {
            (*p).roty -= 10;
            if ((*p).roty % 90 == 0)
                (*p).turningLeft = false;
        }

        if ((*p).roty >= 360)
            (*p).roty -= 360;
        else if ((*p).roty <= 0)
            (*p).roty += 360;

    } else if (upPressed || downPressed) {

        (*p).speedX = 0.05 * sin((*p).roty*PI/180);
		(*p).speedZ = -0.05 * cos((*p).roty*PI/180);

		// efeito de "sobe e desce" ao andar
		(*p).headPosAux += 8.5f;
		if ((*p).headPosAux > 180.0f) {
			(*p).headPosAux = 0.0f;
		}

        if (upPressed) {
            (*p).x += (*p).speedX;
            (*p).z += (*p).speedZ;
        } else {
            (*p).x -= (*p).speedX;
            (*p).z -= (*p).speedZ;
        }

	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		(*p).headPosAux = fmod((*p).headPosAux, 90) - 1 * (*p).headPosAux / 90;
		(*p).headPosAux -= 4.0f;
		if ((*p).headPosAux < 0.0f) {
			(*p).headPosAux = 0.0f;
		}
	}
}
Player player = newPlayer();
/***************** PLAYER END ***********************************************/

/***************** CAMERA ***********************************************/
typedef struct CAMERA{
    Player* player;
} Camera;

Camera newCamera(Player* p){
    printf("New Camera.\n");
    Camera c;
    c.player = p;
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return c;
}
void CameraUpdate(Camera c) {
    //printf("update Camera\n");
    float posYOffset = 0.2;
    Player p = *(c.player);
    float eyeX = p.x;
    float eyeY = p.y + posYOffset + 0.025 * std::abs(sin(p.headPosAux*PI/180));
    float eyeZ = p.z;
    float centerX = eyeX + sin(p.roty*PI/180);
    float centerY = eyeY + cos(p.rotx*PI/180);
    float centerZ = eyeZ - cos(p.roty*PI/180);
	gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);
}
Camera camera = newCamera(&player);
/***************** CAMERA END ***********************************************/

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
    //glmScale(model,sFactor); // USED TO SCALE THE OBJECT
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

    initTexture();

	initModel();

	initLight();

	printf("w - andar \n");
	printf("s - ir pra tras \n");
	printf("mouse - direcao \n");
	printf("r - correr \n");
	printf("c - abaixar \n");
	printf("espaco - pular \n");

}

void initModel() {
	printf("Loading models.. \n");
    bits = LoadDIBitmap("map.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}

    int _x = 0;
    int _z = (int)info->bmiHeader.biHeight - 1;
    bool modelLoaded = false;
    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for(ptr = bits; i > 0; i--, ptr += 3)
    {
        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];
        switch(color)
        {
        case 0x000000:
            break;
        case 0xff0000:
            modelLoaded = C3DObject_Load_New("../../res/rose+vase.obj", &objects[totalObjects].model);
            break;
        case 0x00ff00:
            modelLoaded = C3DObject_Load_New("../../res/f-16.obj", &objects[totalObjects].model);
            break;
        case 0x0000ff:
            modelLoaded = C3DObject_Load_New("../../res/dolphins.obj", &objects[totalObjects].model);
            break;
        case 0xffff00:
            modelLoaded = C3DObject_Load_New("../../res/flowers.obj", &objects[totalObjects].model);
            break;
        case 0xff00ff:
            modelLoaded = C3DObject_Load_New("../../res/porsche.obj", &objects[totalObjects].model);
            break;
        case 0x00ffff:
            modelLoaded = C3DObject_Load_New("../../res/soccerball.obj", &objects[totalObjects].model);
        default:
            printf("Unidentified color.");
        }

        _x += 1;
        if (_x == (int)info->bmiHeader.biWidth){
            _x = 0;
            _z -= 1;
        }
        if(modelLoaded){
            objects[totalObjects].x = _x;
            objects[totalObjects].z = _z;
            totalObjects += 1;
            modelLoaded = false;
        }

    }
	printf("Total objects:  %d. \n \n \n", totalObjects);
}

/**
Initialize the texture using the library bitmap
*/
void initTexture(void)
{
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    bits = LoadDIBitmap("../../res/tiledbronze.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}
    // Figure out the type of texture
    if (info->bmiHeader.biHeight == 1)
      type = GL_TEXTURE_1D;
    else
      type = GL_TEXTURE_2D;

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(type, texture);

    // Create an RGBA image
    rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3)
    {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }

	// Set texture parameters
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(type, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba );


    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);

}

void renderFloor() {
	// set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glEnable(type);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();

    glTranslatef(-(float)planeSize/2.0f, 0.0f, -(float)planeSize/2.0f);

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    int xQuads = 40;
    int zQuads = 40;
    for (int i = 0; i < xQuads; i++) {
        for (int j = 0; j < zQuads; j++) {
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f, 0.0f);   // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 0.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, (j+1) * (float)planeSize/zQuads);

                glTexCoord2f(0.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f((i+1) * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

                glTexCoord2f(1.0f, 1.0f);  // coords for the texture
                glNormal3f(0.0f,1.0f,0.0f);
                glVertex3f(i * (float)planeSize/xQuads, 0.0f, j * (float)planeSize/zQuads);

            glEnd();
        }
    }

	glDisable(type);


	glPopMatrix();
}

void renderScene() {
	glClearColor(backgrundColor[0],backgrundColor[1],backgrundColor[2],backgrundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // limpar o depth buffer

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	CameraUpdate(camera);

    for (i=0; i<totalObjects; i++){
        glPushMatrix();
            glTranslatef(GLfloat(objects[i].x - 3), 0.5, GLfloat(objects[i].z - 3));
            glmDraw(objects[i].model, GLM_SMOOTH);
        glPopMatrix();
    }

    // binds the bmp file already loaded to the OpenGL parameters
    glBindTexture(type, texture);

	renderFloor();
}

/**
Render scene
*/
void mainRender() {
	PlayerUpdate(&player);
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
			upPressed = true;
			break;
		case 115: //s
			downPressed = true;
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
			upPressed = false;
			break;
		case 115: //s
			downPressed = false;
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
	camera = newCamera(&player);
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

	mainInit();

	glutMainLoop();

    return 0;
}
