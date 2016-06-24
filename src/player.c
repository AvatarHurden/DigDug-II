#include "player.h"

#define PI 3.14159265

void loadModel();

Player player;
float rotx = 0;

void newPlayer(){
    printf("New Player.\n");

    player.x = 0.0f;
    player.y = 0.0f;
    player.z = 2.0f;

    player.speedX = 0.0f;
    player.speedY = 0.0f;
    player.speedZ = 0.0f;

    player.roty = 0;
    player.rotx = 90.0f;

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
    if (PlayerEnemyCollision()){
        rotx = 90;
        return;
    } else rotx = 0;
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
	if (getTileXZ(player.x, player.z) == EMPTY)
        PlayerFall();
}

void PlayerTurn(){
    int direction = player.turningRight - player.turningLeft;
    player.roty += 10*direction;
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
    player.speedX = 0.05 * sin(player.roty*PI/180);
    player.speedZ = -0.05 * cos(player.roty*PI/180);
    float newX = player.x + player.speedX*direction;
    float newZ = player.z + player.speedZ*direction;
    if (!hasTypeAt(newX, newZ, 0.075, BLOCK)){
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
    player.y -= 0.1;
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
}

bool PlayerEnemyCollision(){
    int i;
    Enemy e;
    float r = 0.05;
    for (i=0; i<m.numEnemies; i++){
        e = enemies[i];
        printf("%f, %f, %f, %f\n",player.x,e.x,player.z,e.z);
        if (player.x-r < e.x+enemyRadius && player.x+r > e.x-enemyRadius &&
            player.z-r < e.z+enemyRadius && player.z+r > e.z-enemyRadius)
                return true;
    }
    return false;
}

void PlayerDraw() {
    glPushMatrix();

        float eyeY = player.y + 0.119 + 0.025 * std::abs(sin(player.headPosAux*PI/180));

        glTranslatef(GLfloat(player.x), GLfloat(eyeY), GLfloat(player.z));
        glRotatef(180 - player.roty, rotx, 1, 0);
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
