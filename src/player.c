#include "player.h"

#define PI 3.14159265

void loadModel();

Player player;

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
    //printf("Player Update\n");
    if (player.turningRight || player.turningLeft) {
        if (player.turningRight) {
            player.roty += 10;
            if (player.roty % 90 == 0)
                player.turningRight = false;
        } else if (player.turningLeft) {
            player.roty -= 10;
            if (player.roty % 90 == 0)
                player.turningLeft = false;
        }

        if (player.roty >= 360)
            player.roty -= 360;
        else if (player.roty < 0)
            player.roty += 360;

    } else if (player.goingForward || player.goingBackward) {

        player.speedX = 0.05 * sin(player.roty*PI/180);
		player.speedZ = -0.05 * cos(player.roty*PI/180);

		// efeito de "sobe e desce" ao andar
		player.headPosAux += 8.5f;
		if (player.headPosAux > 180.0f) {
			player.headPosAux = 0.0f;
		}
        float newX, newZ;
        if (player.goingForward) {
            newX = player.x + player.speedX;
            newZ = player.z + player.speedZ;
        } else {
            newX = player.x - player.speedX;
            newZ = player.z - player.speedZ;
        }

        if (!hasTypeAt(newX, newZ, 0.075, BLOCK)){
            player.x = newX;
            player.z = newZ;
        }

	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		player.headPosAux = fmod(player.headPosAux, 90) - 1 * player.headPosAux / 90;
		player.headPosAux -= 4.0f;
		if (player.headPosAux < 0.0f) {
			player.headPosAux = 0.0f;
		}
	}
}

void PlayerDraw() {
    glPushMatrix();

        float eyeY = player.y + 0.119 + 0.025 * std::abs(sin(player.headPosAux*PI/180));

        glTranslatef(GLfloat(player.x), GLfloat(eyeY), GLfloat(player.z));
        glRotatef(180 - player.roty, 0, 1, 0);
        glmDraw(player.model, GLM_SMOOTH);
    glPopMatrix();
}

void PlayerHandleInput(unsigned char key, bool pressed){
    switch (key) {
		case 119: //w
			player.goingForward = pressed;
			break;
		case 115: //s
			player.goingBackward = pressed;
			break;
		case 97: //a
            if (pressed && !player.turningRight)
                player.turningLeft = true;
			break;
		case 100: //d
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
