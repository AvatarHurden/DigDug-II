typedef enum {FIRSTPERSON, THIRDPERSON, TOPDOWN} cameraType;
typedef enum {PERSPECTIVE, ORTHOGONAL} projectionType;

typedef struct CAMERA{
    Player* player;
    cameraType type;
    int windowWidth;
    int windowHeight;
    float eyeX, eyeY, eyeZ;
    float centerX, centerY, centerZ;
} Camera;

void newCamera(int width, int height);
void CameraSetType(cameraType type);
void CameraUpdate(cameraType isMiniMap);
void CameraChangeType();
void CameraResize(int w, int h);
void CameraSetType(cameraType type);
void CameraAtPlayer();
void CameraLookAtHorizon();
void CameraBehindPlayer();
void CameraLookInFront();
void CameraAbovePlayer();
void CameraLookAtPlayer();

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
        camera.eyeX = 4;
        camera.eyeY = 1;
        camera.eyeZ = 4;
        camera.centerX = 4;
        camera.centerY = 0;
        camera.centerZ = 4;
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,-1.0);
        camera.type = t;
    }
    else if (player.isDead){
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        camera.eyeX = player.x;
        camera.eyeY = 4;
        camera.eyeZ = player.z;
        camera.centerX = player.x;
        camera.centerY = 0;
        camera.centerZ = player.z;
        gluPerspective(45.0f,camera.windowWidth/camera.windowHeight,0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,1.0);
    } else{
        switch(camera.type){
        case FIRSTPERSON:
            CameraAtPlayer();
            CameraLookAtHorizon();
            gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,1.0,0.0);
            break;
        case THIRDPERSON:
            CameraBehindPlayer();
            CameraLookInFront();
            gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,1.0,0.0);
            break;
        case TOPDOWN:
            CameraAbovePlayer();
            CameraLookAtPlayer();
            gluLookAt(camera.eyeX,camera.eyeY,camera.eyeZ,camera.centerX,camera.centerY,camera.centerZ,0.0,0.0,-1.0);
        }
    }
    GLfloat light_position[] = {camera.eyeX, 10.0, camera.eyeZ, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
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
    camera.eyeX = player.x - 0.01*cosD(player.roty);
    camera.eyeY = player.y + 0.2 + 0.025 * std::abs(sinD(player.headPosAux));
    camera.eyeZ = player.z + 0.01*sinD(player.roty);
}
void CameraLookAtHorizon(){
    camera.centerX = camera.eyeX + cosD(player.roty);
    camera.centerY = camera.eyeY;
    camera.centerZ = camera.eyeZ - sinD(player.roty);
}
void CameraBehindPlayer(){
    camera.eyeX = player.x - 0.5*cosD(player.roty);
    camera.eyeY = player.y + 0.4 + 0.025 * std::abs(sinD(player.headPosAux));
    camera.eyeZ = player.z + 0.5*sinD(player.roty);
}
void CameraLookInFront(){
    camera.centerX = player.x;
    camera.centerY = player.y + 0.3 + 0.025 * std::abs(sinD(player.headPosAux));
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
    CameraSetType(camera.type);
}
