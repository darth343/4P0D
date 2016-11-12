#include "EnemySpawner.h"

Spawner::Spawner()
: m_spawnTimer(0)
{}

Spawner::~Spawner()
{}

void Spawner::Init()
{}

void Spawner::Update(double dt, Player* thePlayer, Player* otherPlay, CMap* m_cMap)
{
    m_spawnTimer -= dt;
    if (m_spawnTimer <= 0 && m_enemyList.size() < 3)
    {
        Spawn();
        m_spawnTimer = 10;
    }

    for (int i = 0; i < m_enemyList.size(); ++i)
    {
        m_enemyList[i]->Update(dt, thePlayer, otherPlay, m_cMap);
    }
}

void Spawner::Spawn()
{
    Enemy* enemy = new Enemy();
    enemy->SetActive(true);
    enemy->SetPos(m_pos + Vector3(Math::RandIntMinMax(-25, 25), Math::RandIntMinMax(-25, 25), 0));
    enemy->SetScale(Vector3(50, 50, 5));
    enemy->SetTarget(Vector3(0,0,0));
    enemy->SetEnemyType(m_TypeToSpawn);
    enemy->SetType(GameObject::ENEMY);
    m_enemyList.push_back(enemy);
}