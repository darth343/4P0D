#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "Tile.h"
#include "Node.h"
#include "Vector3.h"
#include <map>
using namespace std;

class CMap
{
public:
	CMap(void);
	~CMap(void);

	void Init(const int theScreen_Height, const int theScreen_Width, int theTileSize = 32);

	bool LoadMap(const string mapName);
	int GetNumOfTiles_Height(void);		// Get the number of tiles for height of the screen
	int GetNumOfTiles_Width(void);		// Get the number of tiles for width of the screen
	int GetTileSize(void);

	int getScreenWidth(void);		// Get the number of tiles for height of the map
	int getScreenHeight(void);		// Get the number of tiles for width of the map
	
	vector<vector<Tile>> theMap;
	vector<vector<Node *>> theNodeMap;

	int theScreen_Height;
	int theScreen_Width;
	int theNumOfTiles_Height;		// Number of tiles in the Map's height
	int theNumOfTiles_Width;		// Number of tiles in the Map's width
	int theTileSize;

	bool LoadFile(const string mapName);

    std::map<int, Vector3> m_SpawnLocations;
};
