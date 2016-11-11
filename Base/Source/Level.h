#ifndef _LEVEL_H
#define _LEVEL_H

#include "Map.h"
class Level
{
public:
    Level()
    {}
    ~Level()
    {
        if (m_TerrainMap)
        {
            delete m_TerrainMap;
            m_TerrainMap = NULL;
        }
        if (m_SpawnMap)
        {
            delete m_SpawnMap;
            m_SpawnMap = NULL;
        }
    }

    enum LEVELS
    {
        NIL,
        LEVEL1,
        LEVEL2,
        LEVEL3,
    };

    void Init(const int theScreen_Height, const int theScreen_Width, int theTileSize, LEVELS level)
    {
        m_LevelNum = level;

        int levelno = (int)m_LevelNum;

        m_TerrainMap = new CMap();
        m_TerrainMap->Init(theScreen_Height, theScreen_Width, theTileSize);
        m_TerrainMap->LoadMap("Level//" + std::to_string(levelno) + "_terrainMap.csv");

        m_SpawnMap = new CMap();
        m_SpawnMap->Init(theScreen_Height, theScreen_Width, theTileSize);
        m_SpawnMap->LoadMap("Level//" + std::to_string(levelno) + "_spawnMap.csv");
    }

    LEVELS m_LevelNum;
    CMap* m_TerrainMap;     // Map for walls and ground
    CMap* m_SpawnMap;       // Map for monsters and doors etc.
};

#endif