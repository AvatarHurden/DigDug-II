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

void newEnemies();
Enemy newEnemy();
void EnemyUpdateAll();
void EnemyUpdate(Enemy* e);
void EnemyDrawAll();
void EnemyDraw(Enemy e);
void setEnemyPositions(Enemy* e);
void EnemyTurn(Enemy* e);
void EnemyMove(Enemy* e);
void EnemyDecideAction(Enemy* e);


#endif // ENEMY_H_INCLUDED
