typedef struct ENEMY {
    GLMmodel *model;
    int id;
    bool isDead;
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
void EnemyUpdateAll(Position pPos);
void EnemyUpdate(Enemy* e, Position pPos);
void EnemyDrawAll();
void EnemyDraw(Enemy e);
void setEnemyPositions(Enemy* e);
void EnemyShove(Enemy* e);
void EnemyTurn(Enemy* e);
void EnemyMove(Enemy* e);
void EnemyDecideAction(Enemy* e, Position pPos);
Position getEnemyPosition(Enemy e);
bool EnemyEnemyCollision(float x, float z, int id);
void EnemyFall(Enemy* e);
void loadModel(Enemy* e);
bool EnemyCanMoveTo(float x, float z, int id);
void EnemyChasePlayer(Enemy* e, Position pPos);
void EnemyTurnRandom(Enemy* e);
double sinD(float radAngle);
double cosD(float radAngle);

float enemyMoveSpeed = 0.025;
float enemyTurnSpeed = 30; //Deve ser divisor de 90
float enemyRadius = 0.1;
float enemyShoveSpeed = 0.1;
float enemyShoveDeceleration = 0.01;
int aliveEnemies = 0;
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
    e.isDead = false;
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
    aliveEnemies = m.numEnemies;

    return e;
}

void loadModel(Enemy* e) {

    e->model = glmReadOBJ("../../res/models/enemy.obj");

    glmUnitize(e->model);
    glmScale(e->model,0.14); // USED TO SCALE THE OBJECT
    glmFacetNormals(e->model);
    glmVertexNormals(e->model, 90.0);
}

void EnemyUpdateAll(Position pPos){
    int i;
    for (i = 0; i < m.numEnemies; i++)
        EnemyUpdate(&enemies[i], pPos);
}

void EnemyUpdate(Enemy* e, Position pPos) {
    if (getTileXZ(e->x, e->z) == EMPTY)
        EnemyFall(e);
    if (e->shoveSpeedX != 0 || e->shoveSpeedZ != 0){
        EnemyShove(e);
        return;
    }
    if (e->turningRight ^ e->turningLeft) {
        EnemyTurn(e);
        return;
    }
    EnemyDecideAction(e, pPos);
    EnemyMove(e);
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
    int direction = e->turningLeft - e->turningRight;
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
    e->speedX = enemyMoveSpeed * cosD(e->roty);
    e->speedZ = -enemyMoveSpeed * sinD(e->roty);
    float newX = e->x + e->speedX;
    float newZ = e->z + e->speedZ;
    if (EnemyCanMoveTo(newX, newZ, e->id)){
        e->x = newX;
        e->z = newZ;
    } else {
        e->turningRight = true;
    }
}

bool EnemyCanMoveTo(float x, float z, int id){
    return (!hasTypeAt(x, z, enemyRadius, BLOCK) &&
            !hasTypeAt(x, z, enemyRadius, EMPTY) &&
            !hasTypeAt(x, z, enemyRadius, CRACK) &&
            !hasTypeAt(x, z, enemyRadius, HOLE) &&
            !EnemyEnemyCollision(x, z, id));
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
    if (!e->isDead){
        e->isDead = true;
        aliveEnemies--;
        printf("Alive: %d\n",aliveEnemies);
    }
}

bool EnemyEnemyCollision(float x, float z, int id){
    int i;
    Enemy e;
    for (i=0; i<m.numEnemies; i++){
        e = enemies[i];
        if (x-enemyRadius < e.x+enemyRadius && x+enemyRadius > e.x-enemyRadius &&
            z-enemyRadius < e.z+enemyRadius && e.z+enemyRadius > e.z-enemyRadius
            && !e.isDead)
                if (e.id != id) return true;
    }
    return false;
}

void EnemyDecideAction(Enemy* e, Position pPos){
    Position ePos = getPositionXZ(e->x, e->z);
    if (abs(pPos.x - ePos.x) <= 4 || abs(pPos.z - ePos.z) <= 4){ //Se está dentro do raio de visao
        EnemyChasePlayer(e, pPos);
    }
    else if (time(NULL) - e->lastTurnTime > e->walkingTime) {
        printf("VIRADA ALEATORIA\n");
        EnemyTurnRandom(e);
        e->lastTurnTime = time(NULL);
        e->walkingTime = rand() % 3 + 1;
    }
}

void EnemyChasePlayer(Enemy* e, Position pPos){
    //printf("PPERSEGUINDO\n");
    Position ePos = getPositionXZ(e->x, e->z);
    if(abs(pPos.x-ePos.x)>abs(pPos.z-ePos.z)){ //Se a distancia X for maior
        if (pPos.x>ePos.x){ //Se o x do jogador é maior
            switch(e->roty){
            case E: break; // Está virado na direção certa
            case N: e->turningRight = true; break;
            case W: EnemyTurnRandom(e); break;
            case S: e->turningLeft = true; break;
            }
        } else{ // x do inimigo é maior
            switch(e->roty){
            case W: break; // Está virado na direção certa
            case S: e->turningRight = true; break;
            case E: EnemyTurnRandom(e); break;
            case N: e->turningLeft = true; break;
            }
        }
    } else{ //Distancia Z é maior
        if (pPos.z>ePos.z){ //Se o z do jogador é maior
            switch(e->roty){
            case S: break; // Está virado na direção certa
            case E: e->turningRight = true; break;
            case N: EnemyTurnRandom(e); break;
            case W: e->turningLeft = true; break;
            }
        } else{ // z do inimigo é maior
            switch(e->roty){
            case N: break; // Está virado na direção certa
            case W: e->turningRight = true; break;
            case S: EnemyTurnRandom(e); break;
            case E: e->turningLeft = true; break;
            }
        }
    }
}

void EnemyTurnRandom(Enemy* e){
    if (rand() % 2 == 0) e->turningLeft = true;
    else e->turningRight = true;
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
        glRotatef(enemy.roty+90, 0, 1, 0);
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
    int index = 0, i, j;
    for (i = 0; i < m.width; i++)
       for (j = 0; j < m.width; j++)
           if (getTile(i, j) == ENEMY) {
               (e+index)->x = i * m.tileSize + m.tileSize/2.0;
               (e+index)->z = j * m.tileSize + m.tileSize/2.0;
               index++;
           }
}
double sinD(float radAngle){
    return sin(radAngle*PI/180.0);
}
double cosD(float radAngle){
    return cos(radAngle*PI/180.0);
}
