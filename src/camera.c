#include "camera.h"

Camera camera;

float orthoWidth = 7.0/2;

void newCamera(int width, int height){
    printf("New Camera.\n");
    camera.windowWidth = width;
    camera.windowHeight = height;
    camera.type = TOPDOWN; //Vai ser alterado para FIRSTPERSON pela ChangeType
    CameraChangeType();
}
void CameraUpdate() {
    //printf("update Camera\n");
    float posYOffset = 0.2;
    Player p = clonePlayer();

    float eyeX, eyeY, eyeZ;
    float centerX, centerY, centerZ;

    if (p.isDead){
        eyeX = p.x;
        eyeY = 2;
        eyeZ = p.z;

        centerX = p.x;
        centerY = 0;
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,0.0,1.0);
        return;
    }

    if (camera.type == FIRSTPERSON) {
        eyeX = p.x - 0.01*sin(p.roty*PI/180);
        eyeY = p.y + posYOffset + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z + 0.01*cos(p.roty*PI/180);

        centerX = eyeX + sin(p.roty*PI/180);
        centerY = eyeY;// + cos(p.rotx*PI/180);
        centerZ = eyeZ - cos(p.roty*PI/180);

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (camera.type == THIRDPERSON) {
        eyeX = p.x - 0.5*sin(p.roty*PI/180);
        eyeY = p.y + posYOffset + 0.2 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z + 0.5*cos(p.roty*PI/180);

        centerX = p.x;
        centerY = p.y + posYOffset + 0.1 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (camera.type == TOPDOWN) {
        eyeX = p.x;
        eyeY = 1;
        eyeZ = p.z;

        centerX = p.x;
        centerY = 0;
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,0.0,1.0);
    }
}
void CameraChangeType() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch (camera.type){
    case FIRSTPERSON:
        camera.type = THIRDPERSON;
        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        break;
    case THIRDPERSON:
        camera.type = TOPDOWN;
        glOrtho(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth, -1.0f , 2.0f);
        break;
    case TOPDOWN:
        camera.type = FIRSTPERSON;
        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        break;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void CameraResize(int w, int h){
    camera.windowWidth = w;
    camera.windowHeight = h;
}
