#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef enum {EMPTY, NORMAL, BLOCK, HOLE, CRACK, ENEMY, PLAYER} TileType;

typedef struct Map{
    TileType* tiles;
    float tileSize;
    int width;
    int numEnemies;

    GLuint chao_lateral, chao_topo, rest;
} Map;

typedef struct POSITION {
    int x, y;
} Position;

void newMap(char* bitmapFloor, char* bitmapBlocks);
void MapDraw();

TileType getTile(int i, int j);
TileType getTileXZ(float x, float z);
void setTile(int i, int j, TileType tile);


Position* getEnemyPositions();

#endif // MAP_H_INCLUDED
