#include "camera.h"

Camera newCamera(Player* p, int width, int height){
    printf("New Camera.\n");
    Camera c;
    c.player = p;
    c.type = FIRSTPERSON;

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return c;
}
void CameraUpdate(Camera c) {
    //printf("update Camera\n");
    float posYOffset = 0.2;
    Player p = *(c.player);

    float eyeX, eyeY, eyeZ;
    float centerX, centerY, centerZ;

    if (c.type == FIRSTPERSON) {
        eyeX = p.x;
        eyeY = p.y + posYOffset + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z;

        centerX = eyeX + sin(p.roty*PI/180);
        centerY = eyeY + cos(p.rotx*PI/180);
        centerZ = eyeZ - cos(p.roty*PI/180);

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (c.type == THIRDPERSON) {
        eyeX = p.x - 0.5*sin(p.roty*PI/180);
        eyeY = p.y + posYOffset + 0.2 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        eyeZ = p.z + 0.5*cos(p.roty*PI/180);

        centerX = p.x;
        centerY = p.y + posYOffset + 0.1 + 0.025 * std::abs(sin(p.headPosAux*PI/180));
        centerZ = p.z;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);

    } else if (c.type == TOPDOWN) {
        eyeX = 0;
        eyeY = 10;
        eyeZ = 0;

        centerX = 0;
        centerY = 0;
        centerZ = 0;

        gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,0.0,1.0);
    }

}
