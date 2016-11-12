#include "Door.h"

Door::Door()
: m_doorType(UNLOCKABLE)
{}

Door::~Door()
{}

void Door::Init()
{
}

void Door::Update(double dt, CMap* spawnMap)
{
    if (m_doorType == EXIT)
        return;

    if (!m_switch->GetActive() && m_doorType == UNLOCKABLE)
    {
        spawnMap->theMap[m_pos.y / spawnMap->GetTileSize()][m_pos.x / spawnMap->GetTileSize()].SetCollision(false);
        spawnMap->theMap[m_pos.y / spawnMap->GetTileSize()][m_pos.x / spawnMap->GetTileSize()].active = false;
        spawnMap->theMap[m_switch->GetPos().y / spawnMap->GetTileSize()][m_switch->GetPos().x / spawnMap->GetTileSize()].SetCollision(false);
        spawnMap->theMap[m_switch->GetPos().y / spawnMap->GetTileSize()][m_switch->GetPos().x / spawnMap->GetTileSize()].active = false;
        this->SetActive(false);
    }
}