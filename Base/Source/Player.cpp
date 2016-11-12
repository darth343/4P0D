#include "Player.h"
#include <iostream>
#include "Application.h"

std::vector<Projectile*> Player::m_ProjectileList;
std::vector<Tile*> Player::TilesToUpdateList;
bool Player::recoverFog = false;
int Player::points = 0;

Player::Player()
: controllerID(0)
, playerSpeed(120)
, m_attackType(RANGED)
, m_attackDelay(0)
, m_hp(5)
, m_dir(1, 0, 0)
{
}

Player::~Player()
{
}

void Player::Init()
{
}

void Player::Update(double dt, CMap* m_cMap, CMap* spawnMap)
{
    // Movement
	MovePlayer(dt, m_cMap, spawnMap);

    // Update player's projectile
    for (std::vector<Projectile*>::iterator it = m_ProjectileList.begin(); it != m_ProjectileList.end(); ++it)
    {
        Projectile *go = (Projectile *)*it;
        if (go->GetActive())
        {
            go->Update(dt);
        }
    }

    // Fire Delay
    m_attackDelay -= dt;

    // Firing 
    if (Application::IsButtonPressed(controllerID, Application::R2))
    {
        if (m_attackDelay <= 0)
        {
            Attack();
            m_attackDelay = 0.5;
        }
    }
}

void Player::SetMesh(Mesh* mesh)
{
    this->m_mesh = mesh;
}

Mesh* Player::GetMesh()
{
    return m_mesh;
}

void Player::SetPos(const Vector3& pos)
{
    this->m_pos = pos;
}

Vector3 Player::GetPos()
{
    return m_pos;
}

void Player::SetScale(const Vector3& scale)
{
    this->m_scale = scale;
}

Vector3 Player::GetScale()
{
    return m_scale;
}

Vector3 Player::GetCursorPos()
{
    return m_cursorPos;
}

void Player::TakeDamage(int i)
{
    m_hp -= i;
}

void Player::MovePlayer(double dt, CMap* m_cMap, CMap* spawnMap)
{
	Vector3 leftstick = Application::GetLeftStickPos(controllerID);
	Vector3 nextPosition = m_pos + leftstick * playerSpeed * dt;
	Vector3 nextTile = m_pos * (1.f / m_cMap->GetTileSize());
	Vector3 halfNextTile = (m_pos + Vector3((m_cMap->GetTileSize() / 2), (m_cMap->GetTileSize() / 2), 0.f)) * (1.f / m_cMap->GetTileSize());

	if (!leftstick.IsZero())
	{
		if (leftstick.y > 0)
		{
			if (!m_cMap->theMap[nextTile.y + 1][halfNextTile.x].CheckCollide()
				&& !spawnMap->theMap[nextTile.y + 1][halfNextTile.x].CheckCollide())
			{
				m_pos.y = nextPosition.y;
			}
		}
		else if (leftstick.y < 0)
		{
			if (!m_cMap->theMap[nextTile.y][halfNextTile.x].CheckCollide()
				&& !spawnMap->theMap[nextTile.y][halfNextTile.x].CheckCollide())
			{
				m_pos.y = nextPosition.y;// +m_cMap->GetTileSize() * 0.1;;
			}
		}

		if (leftstick.x > 0)
		{
            if (!m_cMap->theMap[halfNextTile.y][nextTile.x + 1].CheckCollide()
				&& !spawnMap->theMap[halfNextTile.y][nextTile.x + 1].CheckCollide())
			{
				m_pos.x = nextPosition.x;
			}
		}
		else if (leftstick.x < 0)
		{
			if (!m_cMap->theMap[halfNextTile.y][nextTile.x].CheckCollide()
				&& !spawnMap->theMap[halfNextTile.y][nextTile.x].CheckCollide())
			{
				m_pos.x = nextPosition.x;
			}
		}
	}
	Vector3 currTile = m_pos * (1.f / m_cMap->GetTileSize());
	currTile.x = (int)currTile.x;
	currTile.y = (int)currTile.y;

	if (prevHeroTile != currTile)
	{
		// Fog Stuff
		int BlockXStart = -5;
		int BlockYStart = -5;

		while (BlockXStart + currTile.x < 0)
		{
			BlockXStart++;
		}
		while (BlockYStart + currTile.y < 0)
		{
			BlockYStart++;
		}

		for (int ystart = BlockYStart; ystart < 5;)
		{
			for (int xstart = BlockXStart; xstart < 5;)
			{
				if(m_cMap->theMap[currTile.y + ystart][currTile.x + xstart].OpacityLevel > 0)
					m_cMap->theMap[currTile.y + ystart][currTile.x + xstart].OpacityLevel--;
				if (spawnMap->theMap[currTile.y + ystart][currTile.x + xstart].OpacityLevel > 0)
					spawnMap->theMap[currTile.y + ystart][currTile.x + xstart].OpacityLevel--;
                if (recoverFog && m_cMap->theMap[currTile.y + ystart][currTile.x + xstart].OpacityLevel < 2 & controllerID == 0)
                {
                    bool inList = false;
                    for (int i = 0; i < TilesToUpdateList.size(); i++)
                    {
                        if (TilesToUpdateList[i] == &m_cMap->theMap[currTile.y + ystart][currTile.x + xstart])
                        {
                            inList = true;
                        }
                    }
                    if (!inList)
                    {
                        TilesToUpdateList.push_back(&m_cMap->theMap[currTile.y + ystart][currTile.x + xstart]);
                        TilesToUpdateList.push_back(&spawnMap->theMap[currTile.y + ystart][currTile.x + xstart]);
                    }
                }
				++xstart;
				if (xstart + currTile.x > m_cMap->GetNumOfTiles_Width() - 1)
					break;
			}
			++ystart;
			if (ystart + currTile.y> m_cMap->GetNumOfTiles_Height() - 1)
				break;
		}
	}
    if (recoverFog && controllerID == 0)
    {
        for (int i = 0; i < TilesToUpdateList.size(); i++)
        {
            if ((currTile - TilesToUpdateList[i]->Pos).Length() < 5)
                continue;
            TilesToUpdateList[i]->timer += dt;
             if (TilesToUpdateList[i]->timer > 0.5f)
            {
                TilesToUpdateList[i]->timer = 0;
                TilesToUpdateList[i]->OpacityLevel += 1;
                if (TilesToUpdateList[i]->OpacityLevel >= 5)
                {
                    TilesToUpdateList[i]->OpacityLevel = 5;
                    TilesToUpdateList.erase(TilesToUpdateList.begin() + i);
                }
            }
        }
    }
	prevHeroTile = currTile;
}

void Player::Attack()
{
	switch (m_attackType)
	{
	case MELEE:
	{
				  Projectile* temp = new Projectile();
				  temp->SetActive(true);
				  temp->SetDmg(1);
				  temp->SetLifetime(0.2);
				  temp->SetPos(this->m_pos);
				  temp->SetScale(Vector3(25, 25, 1));
                  temp->SetPos(this->m_pos + m_scale * 0.5);
				  temp->SetType(GameObject::PROJECTILE_MELEE);

				  Vector3 dir = m_dir;
				  dir.y *= -1;
				  temp->SetVelocity(dir);

				  m_ProjectileList.push_back(temp);
				  break;
	}

	case RANGED:
	{
				   Projectile* temp = new Projectile();
				   temp->SetActive(true);
				   temp->SetDmg(1);
				   temp->SetLifetime(10.f);
				   temp->SetScale(Vector3(10, 10, 1));
				   temp->SetPos(this->m_pos + m_scale * 0.5);
				   temp->SetType(GameObject::PROJECTILE_RANGED);
				   Vector3 dir = m_dir;
				   dir.y *= -1;
				   temp->SetVelocity(dir * 5);

				   m_ProjectileList.push_back(temp);
				   break;
	}
	}
}

void Player::SetHP(float hp)
{
    m_hp = hp;
}

float Player::GetHP()
{
	return m_hp;
}