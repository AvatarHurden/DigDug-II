#include "player.h"

#define PI 3.14159265

Player newPlayer(){
    printf("New Player.\n");
    Player p;
    p.x = 0.0f;
    p.y = 0.0f;
    p.z = 2.0f;
    p.speedX = 0.0f;
    p.speedY = 0.0f;
    p.speedZ = 0.0f;
    p.roty = 0;
    p.rotx = 90.0f;
    p.turningLeft = false;
    p.turningRight = false;
    p.headPosAux = 0.0f;
    return p;
}

void PlayerUpdate(Player* p) {
    //printf("Player Update\n");
    if (p->turningRight || p->turningLeft) {
        if (p->turningRight) {
            p->roty += 10;
            if (p->roty % 90 == 0)
                p->turningRight = false;
        } else if (p->turningLeft) {
            p->roty -= 10;
            if (p->roty % 90 == 0)
                p->turningLeft = false;
        }

        if (p->roty >= 360)
            p->roty -= 360;
        else if (p->roty <= 0)
            p->roty += 360;

    } else if (p->goingForward || p->goingBackward) {

        p->speedX = 0.05 * sin(p->roty*PI/180);
		p->speedZ = -0.05 * cos(p->roty*PI/180);

		// efeito de "sobe e desce" ao andar
		p->headPosAux += 8.5f;
		if (p->headPosAux > 180.0f) {
			p->headPosAux = 0.0f;
		}

        if (p->goingForward) {
            p->x += p->speedX;
            p->z += p->speedZ;
        } else {
            p->x -= p->speedX;
            p->z -= p->speedZ;
        }

	} else {
		// parou de andar, para com o efeito de "sobe e desce"
		p->headPosAux = fmod(p->headPosAux, 90) - 1 * p->headPosAux / 90;
		p->headPosAux -= 4.0f;
		if (p->headPosAux < 0.0f) {
			p->headPosAux = 0.0f;
		}
	}
}