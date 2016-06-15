#include "enemy.h"
#include <time.h>

#define PI 3.14159265

void loadModel(Enemy* e);

Enemy newEnemy(){
    printf("New Enemy.\n");
    Enemy e;
    e.x = 0.0f;
    e.y = 0.0f;
    e.z = 2.0f;

    e.speedX = 0.0f;
    e.speedY = 0.0f;
    e.speedZ = 0.0f;

    e.roty = 0;
    e.rotx = 90.0f;

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

void EnemyUpdate(Enemy* e, Player p) {
    if (e->turningRight || e->turningLeft) {
        e->lastTurnTime = time(NULL);

        if (e->turningRight) {
            e->roty += 10;
            if (e->roty % 90 == 0)
                e->turningRight = false;
        } else if (e->turningLeft) {
            e->roty -= 10;
            if (e->roty % 90 == 0)
                e->turningLeft = false;
        }

        if (e->roty >= 360)
            e->roty -= 360;
        else if (e->roty <= 0)
            e->roty += 360;

    } else {

        e->speedX = -0.025 * sin(e->roty*PI/180);
		e->speedZ = -0.025 * cos(e->roty*PI/180);

        e->x += e->speedX;
        e->z += e->speedZ;

        if (time(NULL) - e->lastTurnTime > e->walkingTime) {
            if (rand() % 2 == 0)
                e->turningLeft = true;
            else
                e->turningRight = true;
            e->walkingTime = rand() % 3 + 1;
        }
	}
}

void EnemyDraw(Enemy enemy) {
    glPushMatrix();

        float eyeY = enemy.y + 0.119;

        glTranslatef(GLfloat(enemy.x), GLfloat(eyeY), GLfloat(enemy.z));
        glRotatef(enemy.roty, 0, 1, 0);
        glmDraw(enemy.model, GLM_SMOOTH);
    glPopMatrix();
}
