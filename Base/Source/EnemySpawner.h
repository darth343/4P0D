#ifndef _ENEMY_SPAWNER_H
#define _ENEMY_SPAWNER_H

#include "Enemy.h"

class Spawner : public GameObject
{
public:
    Spawner();
    ~Spawner();

    Enemy::ENEMY_TYPE m_TypeToSpawn;
    double m_spawnTimer;
    vector<Enemy*>m_enemyList;

    void Init();
    void Update(double dt, Player* thePlayer, Player* otherPlay, CMap* m_cMap);
    void Spawn();


};

#endif