#include "Door.h"

Door::Door()
{}

Door::~Door()
{}

void Door::Init()
{
}

void Door::Update(double dt, CMap* spawnMap)
{
    if (!m_switch->GetActive())
    {
        spawnMap->theMap[m_pos.y / spawnMap->GetTileSize()][m_pos.x / spawnMap->GetTileSize()].SetCollision(false);
        spawnMap->theMap[m_switch->GetPos().y / spawnMap->GetTileSize()][m_switch->GetPos().x / spawnMap->GetTileSize()].SetCollision(false);
        this->SetActive(false);
    }
}