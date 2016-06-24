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
    bool drilling;
    int drillingTime;
    bool turningLeft, turningRight, goingForward, goingBackward;
    float headPosAux;
    unsigned long lastShoveTime;
    bool isDead;
} Player;


void newPlayer();
void PlayerUpdate();
void PlayerDraw();
void setPlayerPosition();
void PlayerTurn();
void PlayerMove();
void PlayerFall();
void PlayerDrill();
bool PlayerEnemyCollision();
void PlayerDie();

#endif // PLAYER_H_INCLUDED
