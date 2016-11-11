#include "Player.h"
#include <iostream>
#include "Application.h"

Player::Player()
: controllerID(0)
, playerSpeed(35)
, m_attackType(RANGED)
, m_attackDelay(0)
{
}

Player::~Player()
{
}

void Player::Init()
{
}

void Player::Update(float worldWidth, float worldHeight, double dt)
{
    // Get the updated cursor positions from Application
    Application::GetCursorPos(&x, &y);

    m_cursorPos.Set(x / Application::GetWindowWidth() * worldWidth, 100.f - (y / Application::GetWindowHeight() * worldHeight), 1.f);

    // Movement
	m_pos += Application::GetLeftStickPos(controllerID) * dt * playerSpeed;

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

void Player::MovePlayer(int movedir, double dt)
{
    switch (movedir)
    {
    case 1:
    {
              m_pos.y += dt * 20;
              break;
    }
    case 2:
    {
              m_pos.y -= dt * 20;
              break;
    }
    case 3:
    {
              m_pos.x -= dt * 20;
              break;
    }
    case 4:
    {
              m_pos.x += dt * 20;
              break;
    }
    }
}

	if (theHeroPosition.x < 0)
	{
		theHeroPosition.x = 0;
	}
	else if (theHeroPosition.x >((m_cMap->GetNumOfTiles_Width() - 1) * m_cMap->GetTileSize() - mapOffset.x) - m_cMap->GetTileSize())
	{
		theHeroPosition.x = ((m_cMap->GetNumOfTiles_Width() - 1) * m_cMap->GetTileSize() - mapOffset.x) - m_cMap->GetTileSize();
	}

void Player::Attack()
	{
		if (mapOffset.y < m_cMap->GetNumOfTiles_Height() * m_cMap->GetTileSize() - 600)
		{
			if (theHeroPosition.y > topBorder)
			{
				float displacement = theHeroPosition.y - topBorder;
				mapOffset.y += displacement;
				theHeroPosition.y = topBorder;
			}
		}
		else
		{
			mapOffset.y = m_cMap->GetNumOfTiles_Height() * m_cMap->GetTileSize() - 600;
		}

		if (mapOffset.y > 0)
		{
			if (theHeroPosition.y < bottomBorder)
			{
				float displacement = bottomBorder - theHeroPosition.y;
				mapOffset.y -= displacement;
				theHeroPosition.y = bottomBorder;
			}
		}
		else
		{
			mapOffset.y = 0;
		}
	}
	else
	{
		if (theHeroPosition.y + m_cMap->GetTileSize() > 600)
		{
			theHeroPosition.y = 600 - m_cMap->GetTileSize();
		}
	}
}

/********************************************************************************
 Hero Update
 ********************************************************************************/
void Player::HeroUpdate(CMap* m_cMap, double dt)
{
	Vector3 leftstick = Application::GetLeftStickPos(controllerID);
	Vector3 nextPosition = theHeroPosition + leftstick * MOVEMENT_SPEED;
	Vector3 nextTile = (nextPosition + mapOffset) * (1.f / m_cMap->GetTileSize());
	if (!leftstick.IsZero())
	{
		if (leftstick.y > 0)
		{
			if (!m_cMap->theMap[nextTile.y + 1][nextTile.x].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide)
			{
				theHeroPosition.y = nextPosition.y;
			}
		}
		else if (leftstick.y < 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide
			&& !m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide)
			{
				theHeroPosition.y = nextPosition.y;
			}
		}

		if (leftstick.x > 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide)
			{
				theHeroPosition.x = nextPosition.x;
			}
		}
		else if (leftstick.x < 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x].shouldCollide)
			{
				theHeroPosition.x = nextPosition.x;
			}
		}

	}
	//if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide && !m_cMap->theMap[nextTile.y ][nextTile.x].shouldCollide && !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide && !m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide)
	//{
	//	theHeroPosition.x = nextPosition.x;
	//}
	//if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide && !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide)
	//{
	//	theHeroPosition = nextPosition;
	//}
	//if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide && !m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide)
	//{
	//	theHeroPosition.x = nextPosition.x;
	//}
	ConstrainHero(800 * 0.2, 800 * 0.8, 600 * 0.8, 600 * 0.2, dt, m_cMap, false, false);
}

void Player::SetHP(float hp)
{
	playerHP = hp;
}

void Player::SetMP(float mp)
{
	playerMP = mp;
}

void Player::SetDMG(float dmg)
{
	playerDamage = dmg;
}

void Player::SetDEF(float def)
{
	playerDefense = def;
}

float Player::GetHP()
{
	return playerHP;
}

float Player::GetDMG()
{
    switch (m_attackType)
    {
    case MELEE:
    {
                  Projectile* temp = new Projectile();
                  temp->SetActive(true);
                  temp->SetDmg(1);
                  temp->SetLifetime(0.1);
                  temp->SetPos(this->m_pos);
                  temp->SetScale(Vector3(5, 5, 1));
                  temp->SetType(GameObject::PROJECTILE_MELEE);

                  Vector3 dir = Application::GetRightStickPos(controllerID);
                  dir.y *= -1;
                  temp->SetVelocity(dir);

                  m_ProjectileList.push_back(temp);
                  break;
                  break;
    }

    case RANGED:
    {
                   Projectile* temp = new Projectile();
                   temp->SetActive(true);
                   temp->SetDmg(1);
                   temp->SetLifetime(0.5);
                   temp->SetPos(this->m_pos);
                   temp->SetScale(Vector3(5, 5, 1));
                   temp->SetType(GameObject::PROJECTILE_RANGED);

                   Vector3 dir = Application::GetRightStickPos(controllerID);
                   dir.y *= -1;
                   temp->SetVelocity(dir);

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
