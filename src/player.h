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
    float headPosAux;
} Player;


Player newPlayer();
void PlayerUpdate(Player* p);
void PlayerDraw(Player* p);

#endif // PLAYER_H_INCLUDED
