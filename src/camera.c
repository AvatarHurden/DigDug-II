#include "camera.h"

Camera camera;

float orthoWidth = 7.0/2;

void newCamera(int width, int height){
    printf("New Camera.\n");
    camera.windowWidth = width;
    camera.windowHeight = height;
    CameraSetType(FIRSTPERSON);
}
void CameraUpdate(bool isMiniMap) {
    if (isMiniMap){
        cameraType t = camera.type;
        CameraSetType(TOPDOWN);
        camera.eyeX = 5;
        camera.eyeY = 1;
        camera.eyeZ = 5;
        camera.centerX = 5;
        camera.centerY = 0;
        camera.centerZ = 5;
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,1.0);
        camera.type = t;
        return;
    }
    if (player.isDead){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        camera.eyeX = player.x;
        camera.eyeY = 4;
        camera.eyeZ = player.z;
        camera.centerX = player.x;
        camera.centerY = 0;
        camera.centerZ = player.z;

        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        return;
    }
    if (camera.type == FIRSTPERSON) {
        CameraAtPlayer();
        CameraLookAtHorizon();
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,1.0,0.0);
    } else if (camera.type == THIRDPERSON) {
        CameraBehindPlayer();
        CameraLookInFront();
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,1.0,0.0);
    } else if (camera.type == TOPDOWN) {
        CameraAbovePlayer();
        CameraLookAtPlayer();
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,1.0);
    }
}

void CameraSetType(cameraType type){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch(type){
    case FIRSTPERSON:
        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        break;
    case THIRDPERSON:
        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        break;
    case TOPDOWN:
        glOrtho(-orthoWidth, orthoWidth, -orthoWidth, orthoWidth, -1.0f , 2.0f);
        break;
    }
    camera.type = type;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CameraChangeType() {
    switch (camera.type){
    case FIRSTPERSON:
        CameraSetType(THIRDPERSON);
        break;
    case THIRDPERSON:
        CameraSetType(TOPDOWN);
        break;
    case TOPDOWN:
        CameraSetType(FIRSTPERSON);
        break;
    }

}

void CameraAtPlayer(){
    camera.eyeX = player.x - 0.01*sin(player.roty*PI/180);
    camera.eyeY = player.y + 0.2 + 0.025 * std::abs(sin(player.headPosAux*PI/180));
    camera.eyeZ = player.z + 0.01*cos(player.roty*PI/180);
}
void CameraLookAtHorizon(){
    camera.centerX = camera.eyeX + sin(player.roty*PI/180);
    camera.centerY = camera.eyeY;// + cos(player.rotx*PI/180);
    camera.centerZ = camera.eyeZ - cos(player.roty*PI/180);
}
void CameraBehindPlayer(){
    camera.eyeX = player.x - 0.5*sin(player.roty*PI/180);
    camera.eyeY = player.y + 0.4 + 0.025 * std::abs(sin(player.headPosAux*PI/180));
    camera.eyeZ = player.z + 0.5*cos(player.roty*PI/180);
}
void CameraLookInFront(){
    camera.centerX = player.x;
    camera.centerY = player.y + 0.3 + 0.025 * std::abs(sin(player.headPosAux*PI/180));
    camera.centerZ = player.z;
}
void CameraAbovePlayer(){
    camera.eyeX = player.x;
    camera.eyeY = 1;
    camera.eyeZ = player.z;
}
void CameraLookAtPlayer(){
    camera.centerX = player.x;
    camera.centerY = player.y;
    camera.centerZ = player.z;
}

void CameraResize(int w, int h){
    camera.windowWidth = w;
    camera.windowHeight = h;
}
