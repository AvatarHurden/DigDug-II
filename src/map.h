#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef enum {EMPTY, NORMAL, BLOCK, HOLE, CRACK, ENEMY, PLAYER} TileType;

typedef struct Map{
    TileType* tiles;
    int width;
    float tileSize;
} Map;

typedef struct POSITION {
    int x, y;
} Position;

Map newMap(char* bitmapFloor, char* bitmapBlocks);
void MapDraw(Map m);

TileType getTile(Map map, int i, int j);
void setTile(Map* map, int i, int j, TileType tile);

Position getPlayerPosition(Map map);
Position* getEnemyPositions(Map map);

#endif // MAP_H_INCLUDED