#include "player.h"

#define PI 3.14159265

void loadModel();

Player player;
float playerMoveSpeed = 0.05;
float playerTurningSpeed = 10; //deve ser divisor de 90
float playerRadius = 0.075;

void newPlayer(){
    printf("New Player.\n");
    player.isDead = false;

    player.x = 0.0f;
    player.y = 0.0f;
    player.z = 2.0f;

    player.speedX = 0.0f;
    player.speedY = 0.0f;
    player.speedZ = 0.0f;

    player.roty = 0;
    player.rotx = 0;

    player.goingForward = false;
    player.goingBackward = false;
    player.turningLeft = false;
    player.turningRight = false;
    player.drilling = false;
    player.drillingTime = 10;
    player.headPosAux = 0.0f;

    setPlayerPosition();
    loadModel();

}

Player clonePlayer(){ return player; }
Player* getPlayer(){ return &player; }

void loadModel() {

    player.model = glmReadOBJ("../../res/goblin_obj.obj");

    glmUnitize(player.model);
    glmScale(player.model,0.12); // USED TO SCALE THE OBJECT
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

    if (player.turningRight ^ player.turningLeft) {
        PlayerTurn();
    } else if (player.drilling) {
        player.headPosAux += 90.0f;
		if (player.headPosAux > 180.0f) {
			player.headPosAux = 0.0f;
		}
		player.drillingTime--;
		if (player.drillingTime == 0) {
            player.drilling = false;
            player.drillingTime = 10;
		}
    } else if (player.goingForward ^ player.goingBackward) {
        PlayerMove();
	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		player.headPosAux = fmod(player.headPosAux, 90) - 1 * player.headPosAux / 90;
		player.headPosAux -= 4.0f;
		if (player.headPosAux < 0.0f) {
			player.headPosAux = 0.0f;
		}
	}

}

void PlayerTurn(){
    int direction = player.turningRight - player.turningLeft;
    player.roty += playerTurningSpeed*direction;
    if (player.roty % 90 == 0){
        player.turningRight = false;
        player.turningLeft = false;
    }
    if (player.roty >= 360)
        player.roty -= 360;
    else if (player.roty < 0)
        player.roty += 360;
}

void PlayerMove(){
    int direction = player.goingForward - player.goingBackward;
    player.speedX = playerMoveSpeed * sin(player.roty*PI/180);
    player.speedZ = -playerMoveSpeed * cos(player.roty*PI/180);
    float newX = player.x + player.speedX*direction;
    float newZ = player.z + player.speedZ*direction;
    if (!hasTypeAt(newX, newZ, playerRadius, BLOCK)){
        player.x = newX;
        player.z = newZ;
    }
    // efeito de "sobe e desce" ao andar
    player.headPosAux += 8.5f;
    if (player.headPosAux > 180.0f) {
        player.headPosAux = 0.0f;
    }
}

void PlayerFall(){
    PlayerDie();
    if (player.y == -0.5)
        return;
    else if (player.y < -0.5){
        player.y = -0.5;
        player.speedY = 0;
    }
    else{
        player.speedY += gravity;
        player.y -= player.speedY;
    }

}

void PlayerDrill() {
    if (player.drilling)
        return;
    player.drilling = true;
    if (getTileXZ(player.x, player.z) != HOLE)
        return;
    int dir = player.roty / 90;
    int zIncrement = 0, xIncrement = 0;
    if (dir == 0)
        zIncrement = -1;
    else if (dir == 1)
        xIncrement = 1;
    else if (dir == 2)
        zIncrement = 1;
    else
        xIncrement = -1;

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

        float eyeY = player.y + 0.119 + 0.025 * std::abs(sin(player.headPosAux*PI/180));

        glTranslatef(GLfloat(player.x), GLfloat(eyeY), GLfloat(player.z));
        glRotatef(player.rotx, 1, 0, 0);
        glRotatef(180 - player.roty, 0, 1, 0);
        glmDraw(player.model, GLM_SMOOTH);
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
