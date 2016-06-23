#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

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
    bool turningLeft, turningRight, goingForward, goingBackward;
    bool digging;
    int diggingTime;
    float headPosAux;
} Player;


void newPlayer();
void PlayerUpdate();
void PlayerDraw();
void setPlayerPosition();

#endif // PLAYER_H_INCLUDED
