typedef struct PLAYER{
    GLMmodel *model;
    float x, y, z;
    float speedX, speedY, speedZ;
    float rotx, rotz;
    int roty;
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
void PlayerUpAndDown(float spd);
void PlayerFall();
void PlayerDrill();
bool PlayerEnemyCollision();
void PlayerDie();
void loadModel();

Player player;
float playerMoveSpeed = 0.05;
float playerTurningSpeed = 30; //deve ser divisor de 90
float playerRadius = 0.075;

void newPlayer(){
    printf("New Player.\n");
    player.isDead = false;

    player.x = 0.0f;
    player.y = 0.0f;
    player.z = 0.0f;

    player.speedX = 0.0f;
    player.speedY = 0.0f;
    player.speedZ = 0.0f;

    player.roty = 90;
    player.rotx = 0;
    player.rotz = 0;
    player.headPosAux = 0.0f;

    player.goingForward = false;
    player.goingBackward = false;
    player.turningLeft = false;
    player.turningRight = false;
    player.drilling = false;
    player.drillingTime = 15;

    setPlayerPosition();
    loadModel();

}

void loadModel() {

    player.model = glmReadOBJ("../../res/models/ED-209.obj");

    glmUnitize(player.model);
    glmScale(player.model,0.14); // USED TO SCALE THE OBJECT
    glmFacetNormals(player.model);
    glmVertexNormals(player.model, 90.0);
}

void PlayerUpdate() {
    if (getTileXZ(player.x, player.z) == EMPTY)
        PlayerFall();
    if (player.isDead)
        return;
    if (PlayerEnemyCollision()){
        PlayerDie();
        return;
    }
    if (aliveEnemies == 0) {
        player.isDead = true;
        return;
    }
    if (player.drilling) {
        PlayerDrill();
    }else if (player.turningRight ^ player.turningLeft) {
        PlayerTurn();
    }else if (player.goingForward ^ player.goingBackward) {
        PlayerMove();
	} else {
		player.headPosAux = fmod(player.headPosAux, 90) - 1 * player.headPosAux / 90;
		player.headPosAux -= 4.0f;
		if (player.headPosAux < 0.0f) {
			player.headPosAux = 0.0f;
		}
	}

}

void PlayerTurn(){
    int direction = player.turningLeft - player.turningRight;
    player.roty += playerTurningSpeed*direction;
    if (player.roty % 90 == 0){
        printf("ANG:%d\n",player.roty);
        player.turningRight = false;
        player.turningLeft = false;
    }
    if (player.roty >= 360)
        player.roty -= 360;
    else if (player.roty < 0)
        player.roty += 360;
}

void PlayerMove(){
    printf("POS x:%f z:%f\n",player.x,player.z);
    int direction = player.goingForward - player.goingBackward;
    player.speedX = playerMoveSpeed*direction * cosD(player.roty);
    player.speedZ = -playerMoveSpeed*direction * sinD(player.roty);
    float newX = player.x + player.speedX;
    float newZ = player.z + player.speedZ;
    if (!hasTypeAt(newX, newZ, playerRadius, BLOCK)){
        player.x = newX;
        player.z = newZ;
    }
    // efeito de "sobe e desce" ao andar
    PlayerUpAndDown(8.5);
}

void PlayerUpAndDown(float spd){
    player.headPosAux += spd;
    if (player.headPosAux > 180.0f) {
        player.headPosAux = 0.0f;
    }
}

void PlayerFall(){
    if (player.isDead) return;
    if (player.y < -1){
        player.speedY = 0;
        PlayerDie();
    }
    else{
        player.speedY += gravity;
        player.y -= player.speedY;
    }

}

void PlayerShoveEnemies() {
    if (time(NULL) - player.lastShoveTime < 2)
        return;

    player.lastShoveTime = time(NULL);
    PlaySound(TEXT("../../res/sounds/shotgun.wav"), NULL, SND_ASYNC|SND_FILENAME);
    float startX, startZ, endX, endZ;

    float blastWidth = 0.3, blastDepth = 3;
    switch (player.roty) {
    case S:
        startX = player.x - blastWidth/2;
        endX = player.x + blastWidth/2;
        startZ = player.z;
        endZ = player.z + blastDepth;
        break;
    case E:
        startX = player.x;
        endX = player.x + blastDepth;
        startZ = player.z - blastWidth/2;
        endZ = player.z + blastWidth/2;
        break;
    case N:
        startX = player.x - blastWidth/2;
        endX = player.x + blastWidth/2;
        startZ = player.z;
        endZ = player.z - blastDepth;
        break;
    case W:
        startX = player.x;
        endX = player.x - blastDepth;
        startZ = player.z - blastWidth/2;
        endZ = player.z + blastWidth/2;
        break;
    }

    if (startX > endX) {
        float temp = endX;
        endX = startX;
        startX = temp;
    }
    if (startZ > endZ) {
        float temp = endZ;
        endZ = startZ;
        startZ = temp;
    }

    int i;
    for (i = 0; i < m.numEnemies; i++)
        if (enemies[i].x > startX && enemies[i].x < endX &&
            enemies[i].z > startZ && enemies[i].z < endZ) {

            enemies[i].shoveSpeedZ = 0;
            enemies[i].shoveSpeedX = 0;
            switch(player.roty){
            case N: enemies[i].shoveSpeedZ = -enemyShoveSpeed; break;
            case S: enemies[i].shoveSpeedZ = enemyShoveSpeed; break;
            case W: enemies[i].shoveSpeedX = -enemyShoveSpeed; break;
            case E: enemies[i].shoveSpeedX = enemyShoveSpeed;
            }
        }
}

void PlayerDrill() {
    PlayerUpAndDown(90);
    player.drillingTime--;
    if (player.drillingTime == 0) {
        player.drilling = false;
        player.drillingTime = 15;
        return;
    }
    if (player.drillingTime < 14)
        return;
    PlaySound(TEXT("../../res/sounds/jackhammer.wav"), NULL, SND_ASYNC|SND_FILENAME);
    if (getTileXZ(player.x, player.z) != HOLE)
        return;
    int zIncrement = 0, xIncrement = 0;
    switch(player.roty){
    case N: zIncrement=-1; break;
    case S: zIncrement=1; break;
    case E: xIncrement=1; break;
    case W: xIncrement=-1; break;
    }

    Position start = getPositionXZ(player.x, player.z);
    do {
        start.x += xIncrement;
        start.z += zIncrement;
        if (getTile(start.x, start.z) == BLOCK)
            return;
        int i;
        for (i = 0; i < m.numEnemies; i++)
            if (getPositionXZ(enemies[i].x, enemies[i].z).x == start.x &&
                getPositionXZ(enemies[i].x, enemies[i].z).z == start.z) {
                return;
            }
    } while (getTile(start.x, start.z) != EMPTY);

    start = getPositionXZ(player.x, player.z);
    start.x += xIncrement;
    start.z += zIncrement;
    while (getTile(start.x, start.z) != EMPTY) {
        setTile(start.x, start.z, CRACK);
        start.x += xIncrement;
        start.z += zIncrement;
    }

    eliminatePartitions();
}

void PlayerDie(){
    player.roty = 0;
    player.rotx = 90.0;
    player.isDead = true;
}

bool PlayerEnemyCollision(){
    int i;
    Enemy e;
    for (i=0; i<m.numEnemies; i++){
        e = enemies[i];
        if (player.x-playerRadius < e.x+enemyRadius && player.x+playerRadius > e.x-enemyRadius &&
            player.z-playerRadius < e.z+enemyRadius && player.z+playerRadius > e.z-enemyRadius)
                return true;
    }
    return false;
}

void PlayerDraw() {
    glPushMatrix();

        float eyeY = player.y + 0.119 + 0.025 * std::abs(sinD(player.headPosAux));

        glTranslatef(player.x, eyeY, player.z);
        glRotatef(player.rotx, 1, 0, 0);
        glRotatef(player.roty+270, 0, 1, 0);
        glmDraw(player.model, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
    glPopMatrix();
}

void PlayerHandleInput(unsigned char key, bool pressed){
    switch (key) {
		case 'w':
			player.goingForward = pressed;
			break;
		case 's':
			player.goingBackward = pressed;
			break;
		case 'a':
            if (pressed && !player.turningRight)
                player.turningLeft = true;
			break;
		case 'd':
		    if (pressed && !player.turningLeft)
                player.turningRight = true;
			break;
        case ' ':
            if (!player.turningLeft && !player.turningRight) player.drilling = true;
            break;
        case 'f':
            PlayerShoveEnemies();
            break;
		default:
			break;
	}
}

void setPlayerPosition() {
    Map m = cloneMap();
    int i, j;
    for (i = 0; i < m.width; i++)
        for (j = 0; j < m.width; j++)
            if (getTile(i, j) == PLAYER) {
                player.x = i * m.tileSize + m.tileSize/2.0;
                player.z = j * m.tileSize + m.tileSize/2.0;
            }
}
