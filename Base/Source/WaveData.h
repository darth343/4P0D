#ifndef WAVE_DATA_H
#define WAVE_DATA_H

#include <vector>
#include <string>

struct WaveData
{
    WaveData(double waitTime, std::string spawn);
    ~WaveData();

    double waitTime;  //buffer time to wait before spawning
    std::string spawn;    //string containing the spawns for this wave
};

bool LoadLevel(const char *file_path, std::vector<WaveData*>& levelData);
void ClearWaveData(std::vector<WaveData*>& levelData);

#endif