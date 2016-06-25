typedef struct ENEMY {
    GLMmodel *model;
    int id;
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
    float shoveSpeedX, shoveSpeedZ;
} Enemy;

void newEnemies();
Enemy newEnemy(int id);
void EnemyUpdateAll();
void EnemyUpdate(Enemy* e);
void EnemyDrawAll();
void EnemyDraw(Enemy e);
void setEnemyPositions(Enemy* e);
void EnemyShove(Enemy* e);
void EnemyTurn(Enemy* e);
void EnemyMove(Enemy* e);
void EnemyDecideAction(Enemy* e);
Position getEnemyPosition(Enemy e);
bool EnemyEnemyCollision(float x, float z, int id);
void EnemyFall(Enemy* e);
void loadModel(Enemy* e);

float enemyMoveSpeed = 0.05;
float enemyTurnSpeed = 30; //Deve ser divisor de 90
float enemyRadius = 0.1;
float enemyShoveSpeed = 0.5;
float enemyShoveDeceleration = 0.005;
Enemy* enemies;

void newEnemies(){
    enemies = (Enemy*) malloc(sizeof(Enemy)*MapGetNumEnemies());
    int i;
	for (i = 0; i < MapGetNumEnemies(); i++)
        enemies[i] = newEnemy(i);
    setEnemyPositions(enemies);
}

Enemy newEnemy(int id){
    printf("New Enemy.\n");
    Enemy e;
    e.id = id;
    e.x = 0.0f;
    e.y = 0.0f;
    e.z = 2.0f;

    e.speedX = 0.0f;
    e.speedY = 0.0f;
    e.speedZ = 0.0f;
    e.shoveSpeedX = 0;
    e.shoveSpeedZ = 0;

    e.roty = 0;
    e.rotx = 0;

    e.turningLeft = false;
    e.turningRight = false;

    loadModel(&e);

    return e;
}

void loadModel(Enemy* e) {

    e->model = glmReadOBJ("../../res/ladybird.obj");

    glmUnitize(e->model);
    glmScale(e->model,0.12); // USED TO SCALE THE OBJECT
    glmFacetNormals(e->model);
    glmVertexNormals(e->model, 90.0);
}

void EnemyUpdateAll(){
    int i;
    for (i = 0; i < m.numEnemies; i++)
        EnemyUpdate(&enemies[i]);
}

void EnemyUpdate(Enemy* e) {
    if (getTileXZ(e->x, e->z) == EMPTY){
        EnemyFall(e);
        return;
    }
    EnemyDecideAction(e);
    if (e->shoveSpeedX != 0 || e->shoveSpeedZ != 0)
        EnemyShove(e);
    if (e->turningRight ^ e->turningLeft) {
        EnemyTurn(e);
    } else {
        EnemyMove(e);
    }
}

void EnemyShove(Enemy* e) {
    e->x += e->shoveSpeedX;
    e->z += e->shoveSpeedZ;

    if (e->shoveSpeedX > 0) {
        e->shoveSpeedX -= enemyShoveDeceleration;
        if (e->shoveSpeedX < 0)
            e->shoveSpeedX = 0;
    } else if (e->shoveSpeedX < 0) {
        e->shoveSpeedX += enemyShoveDeceleration;
        if (e->shoveSpeedX > 0)
            e->shoveSpeedX = 0;
    }

    if (e->shoveSpeedZ > 0) {
        e->shoveSpeedZ -= enemyShoveDeceleration;
        if (e->shoveSpeedZ < 0)
            e->shoveSpeedZ = 0;
    } else if (e->shoveSpeedZ < 0) {
        e->shoveSpeedZ += enemyShoveDeceleration;
        if (e->shoveSpeedZ > 0)
            e->shoveSpeedZ = 0;
    }
}

void EnemyTurn(Enemy* e){
    int direction = e->turningRight - e->turningLeft;
    e->roty += enemyTurnSpeed*direction;
    if (e->roty % 90 == 0){
        e->turningRight = false;
        e->turningLeft = false;
    }
    if (e->roty >= 360)
        e->roty -= 360;
    else if (e->roty < 0)
        e->roty += 360;

}

void EnemyMove(Enemy* e){
    e->speedX = -enemyMoveSpeed * sin(e->roty*PI/180);
    e->speedZ = -enemyMoveSpeed * cos(e->roty*PI/180);
    float newX = e->x + e->speedX;
    float newZ = e->z + e->speedZ;
    if (!hasTypeAt(newX, newZ, enemyRadius, BLOCK) &&
        !hasTypeAt(newX, newZ, enemyRadius, EMPTY) &&
        !EnemyEnemyCollision(newX, newZ, e->id)){
            e->x = newX;
            e->z = newZ;
    } else {
        e->turningRight = true;
        e->lastTurnTime = time(NULL);
    }
}

void EnemyFall(Enemy* e){
    if (e->y == -1)
        return;
    else if (e->y < -1){
        e->y = -1;
        e->speedY = 0;
    }
    else{
        e->speedY += gravity;
        e->y -= e->speedY;
    }

}

bool EnemyEnemyCollision(float x, float z, int id){
    int i;
    Enemy e;
    for (i=0; i<m.numEnemies; i++){
        e = enemies[i];
        if (x-enemyRadius < e.x+enemyRadius && x+enemyRadius > e.x-enemyRadius &&
            z-enemyRadius < e.z+enemyRadius && e.z+enemyRadius > e.z-enemyRadius
            && e.y > -1)
                if (e.id != id) return true;
    }
    return false;
}

void EnemyDecideAction(Enemy* e){
    if (e->shoveSpeedX != 0 || e->shoveSpeedZ != 0)
        return;
    if (time(NULL) - e->lastTurnTime > e->walkingTime) {
        e->lastTurnTime = time(NULL);
        if (rand() % 2 == 0)
            e->turningLeft = true;
        else
            e->turningRight = true;
        e->walkingTime = 99999; //rand() % 3 + 1;
    }
}

void EnemyDrawAll(){
    int i;
    for (i = 0; i < m.numEnemies; i++)
        EnemyDraw(enemies[i]);
}

void EnemyDraw(Enemy enemy) {
    glPushMatrix();

        float eyeY = enemy.y + 0.119;

        glTranslatef(GLfloat(enemy.x), GLfloat(eyeY), GLfloat(enemy.z));
        glRotatef(enemy.roty, 0, 1, 0);
        glmDraw(enemy.model, GLM_SMOOTH);
    glPopMatrix();
}

Position getEnemyPosition(Enemy e){
    Position p;
    p.x = e.x;
    p.z = e.z;
    return p;
}

void setEnemyPositions(Enemy* e) {
    Map m = cloneMap();
    int index = 0, i, j;
    for (i = 0; i < m.width; i++)
       for (j = 0; j < m.width; j++)
           if (getTile(i, j) == ENEMY) {
               (e+index)->x = i * m.tileSize + m.tileSize/2.0;
               (e+index)->z = j * m.tileSize + m.tileSize/2.0;
               index++;
           }
}
