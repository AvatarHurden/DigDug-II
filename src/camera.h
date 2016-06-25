#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

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

#endif // CAMERA_H_INCLUDED
