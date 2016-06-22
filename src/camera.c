#include "camera.h"

Camera newCamera(Player* p, int width, int height){
    printf("New Camera.\n");
    Camera c;
    c.player = p;
    c.windowWidth = width;
    c.windowHeight = height;
    c.type = TOPDOWN; //Vai ser alterado para FIRSTPERSON pela ChangeType
    CameraChangeType(&c);
	return c;
}
void CameraUpdate(Camera c) {
    //printf("update Camera\n");
    float posYOffset = 0.2;
    Player p = *(c.player);

    float eyeX, eyeY, eyeZ;
    float centerX, centerY, centerZ;

    if (c.type == FIRSTPERSON) {
        eyeX = p.x;
        eyeY = p.y + posYOffset + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z;

        centerX = eyeX + sin(p.roty*PI/180);
        centerY = eyeY + cos(p.rotx*PI/180);
        centerZ = eyeZ - cos(p.roty*PI/180);

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (c.type == THIRDPERSON) {
        eyeX = p.x - 0.5*sin(p.roty*PI/180);
        eyeY = p.y + posYOffset + 0.2 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z + 0.5*cos(p.roty*PI/180);

        centerX = p.x;
        centerY = p.y + posYOffset + 0.1 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (c.type == TOPDOWN) {
        eyeX = p.x;
        eyeY = 10;
        eyeZ = p.z;

        centerX = p.x;
        centerY = 0;
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,0.0,1.0);
    }
}
void CameraChangeType(Camera* c) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (c->type){
    case FIRSTPERSON:
        c->type = THIRDPERSON;
        gluPerspective(45.0f,(GLfloat)c->windowWidth/(GLfloat)c->windowHeight,0.1f, 100.0f);
        break;
    case THIRDPERSON:
        c->type = TOPDOWN;
        glOrtho(-5.0f, 5.0f, -5.0f, 5.0f, 0.0f , 11.0f);
        break;
    case TOPDOWN:
        c->type = FIRSTPERSON;
        gluPerspective(45.0f,c->windowWidth/c->windowHeight,0.1f, 100.0f);
        break;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void CameraResize(Camera* c, int w, int h){
    c->windowWidth = w;
    c->windowHeight = h;
}
