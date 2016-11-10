#include <iostream>
#include <fstream>
#include <sstream>
#include "WaveData.h"

WaveData::WaveData(double waitTime, std::string spawn) : waitTime(waitTime), spawn(spawn)
{
}

WaveData::~WaveData()
{
}

bool LoadLevel(const char *file_path, std::vector<WaveData*>& levelData)
{
    std::ifstream fileStream(file_path, std::ios::binary);
    if (!fileStream.is_open()) {
        std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
        return false;
    }

    ClearWaveData(levelData);  //empty the level first, then repopulate with the content

    std::string line;
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);

        std::stringstream dataStream(line);
        std::string data;
        std::getline(dataStream, data, ',');

        if (data == "#") {  //a comment in the file
            continue;
        }

        //first content is buffer time
        double tempTime = std::stod(line);

        //second content is spawns for that wave in a string
        std::getline(dataStream, data);
        std::string tempSpawn = data;

        levelData.push_back( new WaveData(tempTime, tempSpawn) );
    }

    fileStream.close();

    return true;
}

void ClearWaveData(std::vector<WaveData*>& levelData)
{
    while (levelData.size() > 0)
    {
        WaveData *data = levelData.back();
        delete data;
        levelData.pop_back();
    }
}