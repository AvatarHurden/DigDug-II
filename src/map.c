#include "map.h"

void drawBlock(Map m, int i, int j, int height);
void drawWall(Map m, int i, int j, int height, int direction);

GLuint texture;         /* Texture object */

void initTexture(void) {
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap("../../res/tiledbronze.bmp", &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}

    // Create and bind a texture object
    glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

    GLubyte* rgba = (GLubyte *)malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * 4);

    int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    GLubyte *rgbaptr, *ptr;
    for( rgbaptr = rgba, ptr = bits;  i > 0; i--, rgbaptr += 4, ptr += 3) {
            rgbaptr[0] = ptr[2];     // windows BMP = BGR
            rgbaptr[1] = ptr[1];
            rgbaptr[2] = ptr[0];
            rgbaptr[3] = (ptr[0] + ptr[1] + ptr[2]) / 3;
    }

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, info->bmiHeader.biWidth, info->bmiHeader.biHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);

    printf("Textura %d\n", texture);
	printf("Textures ok.\n\n", texture);
}

void loadLowerFloor(Map* m, char* name) {

    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap(name, &info);
    if (bits == (GLubyte *)0) {
        printf("Erro carregando mapa.\nSaindo.\n");
        exit(-1);
    }

    m->tiles = (TileType*) malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * sizeof(TileType));
    m->width = info->bmiHeader.biWidth;

    int _z = 0;
    int _x = (int)info->bmiHeader.biHeight - 1;

    int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    GLubyte *ptr;
    for(ptr = bits; i > 0; i--, ptr += 3) {

        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];

        TileType tile;
        switch(color) {
        case 0x00ff00:
            tile = NORMAL;
            break;
        default:
            tile = EMPTY;
        }

        if (_x >= 0 && _z >= 0)
            setTile(m, _x, _z, tile);

        _z += 1;
        if (_z == (int)info->bmiHeader.biWidth){
            _z = 0;
            _x -= 1;
        }
    }
}

void loadUpperFloor(Map* m, char* name) {

    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap(name, &info);

    if (bits == (GLubyte *)0) {
        printf("Erro carregando mapa.\nSaindo.\n");
        exit(-1);
    } else if (info->bmiHeader.biWidth != m->width) {
        printf("Mapas de tamanhos diferentes.\nSaindo.\n");
        exit(-1);
    }

    int _z = 0;
    int _x = (int)info->bmiHeader.biHeight - 1;

    int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    GLubyte *ptr;
    for(ptr = bits; i > 0; i--, ptr += 3) {

        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];

        TileType tile = getTile(*m, _x, _z);
        if (tile == NORMAL) {
            switch(color) {
            case 0x00ff00:
                tile = BLOCK;
                break;
            case 0xff0000:
                tile = ENEMY;
                break;
            case 0x0000ff:
                tile = PLAYER;
                break;
            case 0x000000:
                tile = HOLE;
                break;
            case 0xffff00:
                tile = CRACK;
                break;
            default:
                TileType tile = NORMAL;
                break;
            }
        }

        if (_x > 0 && _z > 0)
            setTile(m, _x, _z, tile);

        _z += 1;
        if (_z == (int)info->bmiHeader.biWidth){
            _z = 0;
            _x -= 1;
        }
    }
}

Map newMap(char* lower_file_name, char* upper_file_name) {

    Map m;
    initTexture();
    m.tileSize = 0.4;

    loadLowerFloor(&m, lower_file_name);
    printf("loaded bottom");
    loadUpperFloor(&m, upper_file_name);

    int i, j;
    printf("\n");
    for (i = 0; i < m.width + 2; i++)
        printf("-");
    printf("\n");
    for (i = 0; i < m.width; i++) {
        printf("|");
        for (j = 0; j < m.width; j++)
            switch (getTile(m, i, j)) {
            case EMPTY: printf(" "); break;
            case NORMAL: printf("X"); break;
            case BLOCK: printf("#"); break;
            case HOLE: printf("0"); break;
            case CRACK: printf("-"); break;
            case ENEMY: printf("&"); break;
            case PLAYER: printf("P"); break;
            default: printf("@"); break;
            }
        printf("|\n");
    }
    for (i = 0; i < m.width + 2; i++)
        printf("-");

    return m;
}

TileType getTile(Map map, int i, int j) {
    return map.tiles[i + j * map.width];
}

void setTile(Map* map, int i, int j, TileType tile) {
    map->tiles[i + j * map->width] = tile;
}

void MapDraw(Map m) {

    // set things up to render the floor with the texture
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();

    float planeSize = m.tileSize;

	float textureScaleX = 10.0;
	float textureScaleY = 10.0;
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    int xQuads = m.width;
    int zQuads = m.width;
    int divider = 20;
    for (int i = 0; i < xQuads; i++) {
        for (int j = 0; j < zQuads; j++) {
            switch (getTile(m, i, j)) {
                case EMPTY: continue;
                case BLOCK:
                    drawBlock(m, i, j, 1);
                    drawWall(m, i, j, 1, 0);
                    drawWall(m, i, j, 1, 1);
                    drawWall(m, i, j, 1, 2);
                    drawWall(m, i, j, 1, 3);
                    break;
                default:
                    drawBlock(m, i, j, 0);
                }
            if (getTile(m, i, j+1) == EMPTY)
                drawWall(m, i, j, 0, 0);
            if (getTile(m, i+1, j) == EMPTY)
                drawWall(m, i, j, 0, 1);
            if (getTile(m, i, j-1) == EMPTY)
                drawWall(m, i, j, 0, 2);
            if (getTile(m, i-1, j) == EMPTY)
                drawWall(m, i, j, 0, 3);;
        }
    }

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}

void drawBlock(Map m, int i, int j, int level) {
    float size = m.tileSize;

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);   // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(i * size, level*0.2, (j+1) * size);

        glTexCoord2f(0.0f, 0.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f((i+1) * size, level*0.2, (j+1) * size);

        glTexCoord2f(0.0f, 1.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f((i+1) * size, level*0.2, j * size);

        glTexCoord2f(1.0f, 1.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(i * size, level*0.2, j * size);
    glEnd();
}

void drawWall(Map m, int i, int j, int level, int direction) {

    float size = m.tileSize;
    float x1, x2, z1, z2;
    switch (direction) {
    case 0:
        x1 = i, x2 = i+1;
        z1 = j+1, z2 = j+1;
        break;
    case 1:
        x1 = i+1, x2 = i+1;
        z1 = j, z2 = j+1;
        break;
    case 2:
        x1 = i, x2 = i+1;
        z1 = j, z2 = j;
        break;
    case 3:
        x1 = i, x2 = i;
        z1 = j, z2 = j+1;
        break;
    }

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(x1 * size, level*0.2, z1 * size);

        // Inverte a ordem para fazer quadrados corretos
        if (direction == 1 || direction == 3) {
            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x2 * size, (level-1)*0.2, z1 * size);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x1 * size, (level-1)*0.2, z2 * size);
        } else {
            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x1 * size, (level-1)*0.2, z2 * size);

            glTexCoord2f(0.0f, 1.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x2 * size, (level-1)*0.2, z1 * size);
        }

        glTexCoord2f(1.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(x2 * size, level*0.2, z2 * size);

    glEnd();
}

