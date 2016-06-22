#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

typedef enum {FIRSTPERSON, THIRDPERSON, TOPDOWN} cameraType;
typedef enum {PERSPECTIVE, ORTHOGONAL} projectionType;

typedef struct CAMERA{
    Player* player;
    cameraType type;
    int windowWidth;
    int windowHeight;
} Camera;

void newCamera(int width, int height);
void CameraUpdate();
void CameraChangeType();
void CameraResize(int w, int h);

#endif // CAMERA_H_INCLUDED
