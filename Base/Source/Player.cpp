#include "Player.h"
#include <iostream>
#include "Application.h"

std::vector<Projectile*> Player::m_ProjectileList;

Player::Player()
: controllerID(0)
, playerSpeed(2)
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

void Player::Update(double dt, CMap* m_cMap)
{
    // Movement
	MovePlayer(dt, m_cMap);

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

void Player::MovePlayer(double dt, CMap* m_cMap)
{
	Vector3 leftstick = Application::GetLeftStickPos(controllerID);
	Vector3 nextPosition = m_pos + leftstick * playerSpeed;
	Vector3 nextTile = nextPosition * (1.f / m_cMap->GetTileSize());
	if (!leftstick.IsZero())
	{
		if (leftstick.y > 0)
		{
			if (!m_cMap->theMap[nextTile.y + 1][nextTile.x].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide)
			{
				m_pos.y = nextPosition.y;
			}
		}
		else if (leftstick.y < 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide
				&& !m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide)
			{
				m_pos.y = nextPosition.y;
			}
		}

		if (leftstick.x > 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x + 1].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x + 1].shouldCollide)
			{
				m_pos.x = nextPosition.x;
			}
		}
		else if (leftstick.x < 0)
		{
			if (!m_cMap->theMap[nextTile.y][nextTile.x].shouldCollide
				&& !m_cMap->theMap[nextTile.y + 1][nextTile.x].shouldCollide)
			{
				m_pos.x = nextPosition.x;
			}
		}

	}

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
				   temp->SetPos(this->m_pos + m_scale * 0.5);
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
}