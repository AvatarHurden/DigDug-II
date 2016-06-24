#include "map.h"

void drawBlock(int i, int j, int height, int rotation);
void drawHole(int i, int j);
void drawCrack(int i, int j);
void drawWall(int i, int j, int height, int direction);

Map m;

Map* getMap(){ return &m; }
Map cloneMap(){ return m; }
int MapGetNumEnemies() { return m.numEnemies; }

float blockSize = 0.4;

void initTexture(char* name, GLuint* texture) {
    printf ("\nLoading texture..\n");
    // Load a texture object (256x256 true color)
    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap(name, &info);
    if (bits == (GLubyte *)0) {
		printf ("Error loading texture!\n\n");
		return;
	}

    // Create and bind a texture object
    glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

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

    printf("Textura %d\n", *texture);
	printf("Textures ok.\n\n");
}

void loadLowerFloor(char* name) {

    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap(name, &info);
    if (bits == (GLubyte *)0) {
        printf("Erro carregando mapa.\nSaindo.\n");
        exit(-1);
    }

    m.tiles = (TileType*) malloc(info->bmiHeader.biWidth * info->bmiHeader.biHeight * sizeof(TileType));
    m.width = info->bmiHeader.biWidth;

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
            setTile(_x, _z, tile);

        _z += 1;
        if (_z == (int)info->bmiHeader.biWidth){
            _z = 0;
            _x -= 1;
        }
    }
}

void loadUpperFloor(char* name) {

    BITMAPINFO *info;
    GLubyte* bits = LoadDIBitmap(name, &info);

    if (bits == (GLubyte *)0) {
        printf("Erro carregando mapa.\nSaindo.\n");
        exit(-1);
    } else if (info->bmiHeader.biWidth != m.width) {
        printf("Mapas de tamanhos diferentes.\nSaindo.\n");
        exit(-1);
    }

    int _z = 0;
    int _x = (int)info->bmiHeader.biHeight - 1;

    int i = info->bmiHeader.biWidth * info->bmiHeader.biHeight;
    GLubyte *ptr;
    for(ptr = bits; i > 0; i--, ptr += 3) {

        int color = (ptr[2] << 16) + (ptr[1] << 8) + ptr[0];

        TileType tile = getTile(_x, _z);
        if (tile == NORMAL) {
            switch(color) {
            case 0x00ff00:
                tile = BLOCK;
                break;
            case 0xff0000:
                tile = ENEMY;
                m.numEnemies++;
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
            setTile(_x, _z, tile);

        _z += 1;
        if (_z == (int)info->bmiHeader.biWidth){
            _z = 0;
            _x -= 1;
        }
    }
}

void newMap() {
    char* lower_file_name = "../../res/lower.bmp";
    char* upper_file_name = "../../res/upper.bmp";

    initTexture("../../res/normallateral.bmp", &m.chao_lateral);
    initTexture("../../res/normaltopo.bmp", &m.chao_topo);
    initTexture("../../res/blocolateral.bmp", &m.bloco_lateral);
    initTexture("../../res/blocotopo.bmp", &m.bloco_topo);
    initTexture("../../res/buraco.bmp", &m.buraco);
    initTexture("../../res/buraco1rachadura.bmp", &m.buraco_1_rachadura);
    initTexture("../../res/buraco2rachadurasparalelas.bmp", &m.buraco_2_rachaduras_paralelas);
    initTexture("../../res/buraco2rachadurasperpendiculares.bmp", &m.buraco_2_rachaduras_perpendiculares);
    initTexture("../../res/buraco3rachaduras.bmp", &m.buraco_3_rachaduras);
    initTexture("../../res/buraco4rachaduras.bmp", &m.buraco_4_rachaduras);
    initTexture("../../res/rachadura.bmp", &m.rachadura);
    initTexture("../../res/rachaduracruzada.bmp", &m.rachadura_cruzada);
    m.tileSize = 0.4;
    m.numEnemies = 0;

    loadLowerFloor(lower_file_name);
    printf("loaded bottom");
    loadUpperFloor(upper_file_name);

    int i, j;
    printf("\n");
    for (i = 0; i < m.width + 2; i++)
        printf("-");
    printf("\n");
    for (i = 0; i < m.width; i++) {
        printf("|");
        for (j = 0; j < m.width; j++)
            switch (getTile(i, j)) {
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

}

TileType getTile(int i, int j) {
    return m.tiles[i + j * m.width];
}

void setTile(int i, int j, TileType tile) {
    m.tiles[i + j * m.width] = tile;
}

TileType getTileXZ(float x, float z){
    int L = (int)(x/m.tileSize);
    int C = (int)(z/m.tileSize);
    return m.tiles[L + C*m.width];
}

Position getPositionXZ(float x, float z) {
    Position p;
    p.x = (int)(x/m.tileSize);
    p.z = (int)(z/m.tileSize);
    return p;
}

bool PositionEquals(Position p1, Position p2){
    return (p1.x == p2.x && p1.z == p2.z);
}

bool hasTypeAt(float x, float z, float radius, TileType type) {

    return getTileXZ(x+radius, z+radius) == type ||
        getTileXZ(x-radius, z+radius) == type ||
        getTileXZ(x-radius, z-radius) == type ||
        getTileXZ(x+radius, z-radius) == type;
}

void MapDraw() {

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
            switch (getTile(i, j)) {
                case EMPTY: continue;
                case HOLE:
                    glBindTexture(GL_TEXTURE_2D, m.buraco);
                    drawHole(i, j);
                    break;
                case CRACK:
                    drawCrack(i, j);
                    break;
                case BLOCK:
                    glBindTexture(GL_TEXTURE_2D, m.bloco_topo);
                    drawBlock(i, j, 1, 0);
                    drawWall(i, j, 1, 0);
                    drawWall(i, j, 1, 1);
                    drawWall(i, j, 1, 2);
                    drawWall(i, j, 1, 3);
                    break;
                default:
                    glBindTexture(GL_TEXTURE_2D, m.chao_topo);
                    drawBlock(i, j, 0, 0);
                }
            if (getTile(i, j+1) == EMPTY)
                drawWall(i, j, 0, 0);
            if (getTile(i+1, j) == EMPTY)
                drawWall(i, j, 0, 1);
            if (getTile(i, j-1) == EMPTY)
                drawWall(i, j, 0, 2);
            if (getTile(i-1, j) == EMPTY)
                drawWall(i, j, 0, 3);;
        }
    }

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}

void drawBlock(int i, int j, int level, int rotation) {
    float size = m.tileSize;

    glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glTranslatef(0.5,0.5,0.0);
        glRotatef(rotation,0.0,0.0,1.0);
        glTranslatef(-0.5,-0.5,0.0);
    glMatrixMode(GL_MODELVIEW);

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);   // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(i * size, level*blockSize, (j+1) * size);

        glTexCoord2f(0.0f, 0.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f((i+1) * size, level*blockSize, (j+1) * size);

        glTexCoord2f(0.0f, 1.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f((i+1) * size, level*blockSize, j * size);

        glTexCoord2f(1.0f, 1.0f);  // coords for the texture
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(i * size, level*blockSize, j * size);
    glEnd();
}

void drawHole(int i, int j) {

    int cracks = 0;
    if (getTile(i+1, j) == CRACK) cracks++;
    if (getTile(i, j+1) == CRACK) cracks++;
    if (getTile(i-1, j) == CRACK) cracks++;
    if (getTile(i, j-1) == CRACK) cracks++;

    GLuint texture;
    int rotation = 0;
    switch (cracks) {
    case 0: texture = m.buraco; break;
    case 1:
        texture = m.buraco_1_rachadura;
        if (getTile(i+1, j) == CRACK) rotation = 90;
        if (getTile(i, j+1) == CRACK) rotation = 0;
        if (getTile(i-1, j) == CRACK) rotation = -90;
        if (getTile(i, j-1) == CRACK) rotation = 180;
        break;
    case 3:
        texture = m.buraco_3_rachaduras;
        if (getTile(i+1, j) != CRACK) rotation = 0;
        if (getTile(i, j+1) != CRACK) rotation = -90;
        if (getTile(i-1, j) != CRACK) rotation = 180;
        if (getTile(i, j-1) != CRACK) rotation = 90;
        break;
    case 4:
        texture = m.buraco_4_rachaduras; break;
    case 2:
        if (getTile(i+1, j) == getTile(i-1, j) ||
            getTile(i, j+1) == getTile(i, j-1)) {
            texture = m.buraco_2_rachaduras_paralelas;
            rotation = (getTile(i+1, j) == CRACK) ? 90 : 0;
        } else
            texture = m.buraco_2_rachaduras_perpendiculares;
            if (getTile(i+1, j) == CRACK && getTile(i, j+1) == CRACK) rotation = 90;
            if (getTile(i, j+1) == CRACK && getTile(i-1, j) == CRACK) rotation = 0;
            if (getTile(i-1, j) == CRACK && getTile(i, j-1) == CRACK) rotation = -90;
            if (getTile(i, j-1) == CRACK && getTile(i+1, j) == CRACK) rotation = 180;

        break;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    drawBlock(i, j, 0, rotation);

}

void drawCrack(int i, int j) {

    int neighbors = 0;
    if (getTile(i+1, j) == CRACK || getTile(i-1, j) == CRACK ||
        getTile(i+1, j) == HOLE || getTile(i-1, j) == HOLE)
            neighbors++;
    if (getTile(i, j+1) == CRACK || getTile(i, j-1) == CRACK ||
        getTile(i, j+1) == HOLE || getTile(i, j-1) == HOLE)
            neighbors++;

    GLuint texture;
    int rotation = 0;
    switch (neighbors) {
    case 1:
        texture = m.rachadura;
        if (getTile(i+1, j) == CRACK || getTile(i-1, j) == CRACK ||
            getTile(i+1, j) == HOLE || getTile(i-1, j) == HOLE) rotation = 90;
        break;
    case 2: texture = m.rachadura_cruzada; break;
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    drawBlock(i, j, 0, rotation);

}

void drawWall(int i, int j, int level, int direction) {

   if (level == 1)
        glBindTexture(GL_TEXTURE_2D, m.bloco_lateral);
    else
        glBindTexture(GL_TEXTURE_2D, m.chao_lateral);

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


    glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glTranslatef(0.5,0.5,0.0);
        glRotatef(0,0.0,0.0,1.0);
        glTranslatef(-0.5,-0.5,0.0);
    glMatrixMode(GL_MODELVIEW);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(x1 * size, level*blockSize, z1 * size);

        // Inverte a ordem para fazer quadrados corretos
        if (direction == 1 || direction == 3) {
            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x2 * size, (level-1)*blockSize, z1 * size);

            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x1 * size, (level-1)*blockSize, z2 * size);
        } else {
            glTexCoord2f(0.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x1 * size, (level-1)*blockSize, z2 * size);

            glTexCoord2f(1.0f, 0.0f);
            glNormal3f(0.0f,1.0f,0.0f);
            glVertex3f(x2 * size, (level-1)*blockSize, z1 * size);
        }

        glTexCoord2f(1.0f, 1.0f);
        glNormal3f(0.0f,1.0f,0.0f);
        glVertex3f(x2 * size, level*blockSize, z2 * size);

    glEnd();
}

void floodFill2(int matrix[][20], int i, int j, int value) {

    if (matrix[i][j] == value)
        return;
    if (matrix[i][j] != 0)
        return;
    matrix[i][j] = value;
//
//    printf("\n");
//    printf("\n");
//     for (i = m.width-1; i >= 0; i--) {
//        for (j = 0; j < m.width; j++)
//            printf("%2d", matrix[i][j]);
//        printf("\n");
//     }

    floodFill2(matrix, i+1, j, value);
    floodFill2(matrix, i, j+1, value);
    floodFill2(matrix, i-1, j, value);
    floodFill2(matrix, i, j-1, value);
}

void findPartitions() {

    int parts[m.width][20];
    int i, j;
    for (i = 0; i < m.width; i++)
        for (j = 0; j < m.width; j++)
            if (getTile(i, j) == EMPTY)
                parts[i][j] = -2;
            else if (getTile(i, j) == CRACK || getTile(i, j) == HOLE)
                parts[i][j] = -1;
            else
                parts[i][j] = 0;

    bool isPartitionable = false;
    int seedI, seedJ;
    for (i = 0; i < m.width; i++)
        for (j = 0; j < m.width; j++)
            if (parts[i][j] == 0) {
                isPartitionable = true;
                seedI = i;
                seedJ = j;
            }

    int partitions = 1;
    while (isPartitionable) {
        floodFill2(parts, seedI, seedJ, partitions);

        isPartitionable = false;
        for (i = 0; i < m.width; i++)
            for (j = 0; j < m.width; j++)
                if (parts[i][j] == 0) {
                    isPartitionable = true;
                    seedI = i;
                    seedJ = j;
                }
        partitions++;
    }

     for (i = m.width-1; i >= 0; i--) {
        for (j = 0; j < m.width; j++)
            printf("%2d", parts[i][j]);
        printf("\n");
     }
}

