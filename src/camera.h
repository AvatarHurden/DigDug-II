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

Camera newCamera(Player* p, int width, int height);
void CameraUpdate(Camera c);
void CameraChangeType(Camera* c);
void CameraResize(Camera* c, int w, int h);

#endif // CAMERA_H_INCLUDED
