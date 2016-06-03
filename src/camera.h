#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

typedef struct CAMERA{
    Player* player;
} Camera;

Camera newCamera(Player* p, int width, int height);
void CameraUpdate(Camera c);

#endif // CAMERA_H_INCLUDED
