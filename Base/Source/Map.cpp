#include "Map.h"
#include <iostream>
#include <sstream>
CMap::CMap(void)
: theScreen_Height(0)
, theScreen_Width(0)
, theNumOfTiles_Height(0)
, theNumOfTiles_Width(0)
, theTileSize(0)
{
	//theMap.clear();
    //theNodeMap.clear();
}

CMap::~CMap(void)
{
	theMap.clear();
    theNodeMap.clear();
}


void CMap::Init(const int theScreen_Height, const int theScreen_Width, int theTileSize)
{
	this->theScreen_Height = theScreen_Height;
	this->theScreen_Width = theScreen_Width;
	this->theTileSize = theTileSize;
}

bool CMap::LoadMap(const string mapName)
{
	if (LoadFile(mapName) == true)
	{
		printf("Map (%s) has been successfully loaded!\n", mapName.c_str());
		return true;
	}
	printf("Map (%s) can't be accessed. Are you in the correct directory?\n", mapName.c_str());
	return false;
}

bool CMap::LoadFile(const string mapName)
{
	int theLineCounter = 0;
	int theMaxNumOfColumns = 0;

	ifstream file(mapName.c_str());
	if (file.is_open())
	{
		//Update Map Height && Map Width
		{
			string tempString;
			int NumberOfRows = 0;
			int NumberOfCols = 1;
			getline(file, tempString);
			while (getline(file, tempString))
			{
				if (tempString[0] == '#')
					break;
				NumberOfRows++;
			}
			theNumOfTiles_Height = NumberOfRows;
			file.clear();
			file.seekg(0);
			getline(file, tempString);
			getline(file, tempString);
			for (int i = 0; i < tempString.size(); ++i)
			{
				if (tempString[i] == ',')
				++NumberOfCols;
			}
			theNumOfTiles_Width = NumberOfCols;
		}
		//Allocate Memory Dynamically For Map
		{
		theMap.resize(theNumOfTiles_Height);
		for (int i = 0; i < theNumOfTiles_Height; ++i)
			theMap[i].resize(theNumOfTiles_Width);

		theNodeMap.resize(theNumOfTiles_Height);
		for (int i = 0; i < theNumOfTiles_Height; ++i)
			theNodeMap[i].resize(theNumOfTiles_Width);
		}
		file.clear();
		file.seekg(0);
		//Populate Map
		{
			int x = 0, y = 0;
			string tempString;
			getline(file, tempString);
			for (y = theNumOfTiles_Height - 1; y >= 0; --y)
			{
				std::stringstream ss;
				getline(file, tempString);
				ss << tempString;
				for (x = 0; x < theNumOfTiles_Width; ++x)
				{
					if (x == theNumOfTiles_Width-1)
						getline(ss, tempString, '\n');
					else
						getline(ss, tempString, ',');
					int ID = std::stoi(tempString);
 
					Tile tempTile(Vector3(x, y), ID);
					theMap[y][x] = tempTile;

                    // Save pos of monsters, doors, switches etc

                    Vector3 tempSpawn;
                    tempSpawn.Set(x * theTileSize, y * theTileSize, 1);

                    // 5 == Melee Monster
                    // 6 == Ranged Monster
                    // > 100 == Door/Switch pair

                    if (ID >= 5)
                    {
                        m_SpawnLocations.insert(std::pair<int, Vector3>(ID, tempSpawn));
                        if (ID > 1000)
                        {
                            int pairNum = ID / 1000 % 10; // Pair Number
                            int objectType = ID / 100 % 10; // Object: 1 == Door, 2 == Switch
                            int doorNum = ID / 10 % 10; // If applicable, 1 == first door, 2 == second door else 0
                            int weaknessType = ID % 10;  // Weakness: 1 == BOTH, 2 == MELEE, 3 == RANGED

                            if (objectType == 1)
                            {
                                theMap[y][x].active = true;
                                theMap[y][x].shouldCollide = true;
                            }
                        }
                    }
				}
			}
		}
		//Set Collision for Tiles
		{
			string tempString;
			int NumberOfRows = 0;
			int NumberOfCols = 0;
			getline(file, tempString);
			for (int y = theNumOfTiles_Height - 1; y >= 0; --y)
			{
				std::stringstream ss;
				getline(file, tempString);
				ss << tempString;
				for (int x = 0; x < theNumOfTiles_Width; ++x)
				{
					if (x == theNumOfTiles_Width-1)
						getline(ss, tempString, '\n');
					else
						getline(ss, tempString, ',');
					bool Collidable = stoi(tempString);
					theMap[y][x].SetCollision(Collidable);
				}
			}
		}
		//Populate Node Map for PathFinding
		int Node_ID = 0;
		for (int y = 0; y < theNumOfTiles_Height; ++y)
		{
			for (int x = 0; x < theNumOfTiles_Width; ++x)
			{
				theNodeMap[y][x] = new Node(theMap[y][x]);
				theNodeMap[y][x]->nodeID = Node_ID;
				++Node_ID;
			}
		}
	}
	return true;
}

int CMap::GetNumOfTiles_Height(void)
{
	return theNumOfTiles_Height;
}

int CMap::GetNumOfTiles_Width(void)
{
	return theNumOfTiles_Width;
}

int CMap::GetTileSize(void)
{
	return theTileSize;
}

// Get the number of tiles for height of the map
int CMap::getScreenHeight()
{
	return theScreen_Height;
}

// Get the number of tiles for height of the map
int CMap::getScreenWidth()
{
	return theScreen_Width;
}
