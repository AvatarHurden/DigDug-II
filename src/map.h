#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef enum {EMPTY, NORMAL, BLOCK, HOLE, CRACK, ENEMY, PLAYER} TileType;


typedef struct Map{
    TileType* tiles;
    float tileSize;
    int width;
    int numEnemies;

    GLuint chao_lateral, chao_topo, bloco_lateral, bloco_topo;
    GLuint buraco, buraco_1_rachadura, buraco_2_rachaduras_paralelas;
    GLuint buraco_2_rachaduras_perpendiculares, buraco_3_rachaduras, buraco_4_rachaduras;
    GLuint rachadura, rachadura_cruzada;
} Map;

typedef struct POSITION {
    int x, z;
} Position;

void newMap();
void MapDraw();

TileType getTile(int i, int j);
TileType getTileXZ(float x, float z);
Position getPositionXZ(float x, float z);
bool hasTypeAt(float x, float z, float radius, TileType type);
void setTile(int i, int j, TileType tile);
bool PositionEquals(Position p1, Position p2);

void eliminatePartitions();

Position* getEnemyPositions();

#endif // MAP_H_INCLUDED
