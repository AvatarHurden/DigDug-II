#include "map.h"

//GLubyte *bits;

TileType getTile(Map map, int i, int j) {
    return map.tiles[i + j * map.width];
}

void setTile(Map *map, int i, int j, TileType tile) {
    map->tiles[i + j * map->width] = tile;
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
    m.tileSize = 8;

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
}



