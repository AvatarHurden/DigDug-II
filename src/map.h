#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

typedef enum {EMPTY, NORMAL, BLOCK, HOLE, CRACK, ENEMY, PLAYER} TileType;

typedef struct Map{
    TileType* tiles;
    int width;
    int tileSize;
} Map;

Map newMap(char* bitmapFloor, char* bitmapBlocks);
TileType getTile(Map map, int i, int j);
void setTile(Map map, int i, int j, TileType tile);

#endif // MAP_H_INCLUDED
