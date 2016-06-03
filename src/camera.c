#include "camera.h"

Camera newCamera(Player* p, int width, int height){
    printf("New Camera.\n");
    Camera c;
    c.player = p;
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
    float eyeX = p.x;
    float eyeY = p.y + posYOffset + 0.025 * std::abs(sin(p.headPosAux*PI/180));
    float eyeZ = p.z;
    float centerX = eyeX + sin(p.roty*PI/180);
    float centerY = eyeY + cos(p.rotx*PI/180);
    float centerZ = eyeZ - cos(p.roty*PI/180);
	gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,0.0,1.0,0.0);
}
