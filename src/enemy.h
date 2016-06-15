#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

typedef struct ENEMY {
    GLMmodel *model;
    float x;
    float y;
    float z;
    float speedX;
    float speedY;
    float speedZ;
    int roty;
    float rotx;
    bool turningLeft, turningRight;
    unsigned long lastTurnTime;
    int walkingTime;
} Enemy;

Enemy newEnemy();
void EnemyUpdate(Enemy* e, Player p);
void EnemyDraw(Enemy e);

#endif // ENEMY_H_INCLUDED
