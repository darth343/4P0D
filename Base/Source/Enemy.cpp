#include "Enemy.h"
//std::vector<Projectile*> Enemy::m_ProjectileList;
Vector3 Enemy::prevPlayerTile;
Enemy::Enemy()
: m_attackDelay(0)
, m_hp(1)
, speed(100)
, findTargetDelay(-1)
{}

Enemy::~Enemy()
{}

void Enemy::Init()
{
}

void Enemy::MoveTo(double dt, Tile nextTile, int TileSize)
{
	if (nextTile.Pos.x * TileSize < m_pos.x)
	{
		m_pos.x -= dt * speed;
		if (nextTile.Pos.x * TileSize > m_pos.x)
		{
			m_pos.x = nextTile.Pos.x * TileSize;
		}
	}
	else if	(nextTile.Pos.x * TileSize > m_pos.x)
	{
		m_pos.x += dt * speed;
		if (nextTile.Pos.x * TileSize < m_pos.x)
		{
			m_pos.x = nextTile.Pos.x * TileSize;
		}
	}
	if (nextTile.Pos.y * TileSize < m_pos.y)
	{
		m_pos.y -= dt * speed;
		if (nextTile.Pos.y * TileSize > m_pos.y)
		{
			m_pos.y = nextTile.Pos.y * TileSize;
		}
	}
	else if (nextTile.Pos.y * TileSize > m_pos.y)
	{
		m_pos.y += dt * speed;
		if (nextTile.Pos.y * TileSize < m_pos.y)
		{
			m_pos.y = nextTile.Pos.y * TileSize;
		}
	}
	if (nextTile.Pos.x * TileSize == m_pos.x && nextTile.Pos.y * TileSize == m_pos.y)
		pathfinder.pathToEnd.pop_back();
}

void Enemy::MoveToPlayer(double dt, Player* thePlayer, CMap* m_cMap)
{
	Vector3 currentTile = this->m_pos * (1.f / m_cMap->GetTileSize());
	Vector3 playerTile = thePlayer->GetPos() * (1.f / m_cMap->GetTileSize());
	pathfinder.FindPath(m_cMap->theMap[currentTile.y][currentTile.x], m_cMap->theMap[playerTile.y][playerTile.x], m_cMap);
	if (pathfinder.found)
	{
		Tile nextTile = pathfinder.pathToEnd.back();
		MoveTo(dt, nextTile, m_cMap->GetTileSize());
		if (pathfinder.pathToEnd.size() > 1)
		{
			if (abs((prevPlayerTile - playerTile).Length()) > 1)
			{
				pathfinder.initializedStartandEnd = false;
				pathfinder.found = false;
			}
		}
		else
		{
			pathfinder.initializedStartandEnd = false;
			pathfinder.found = false;
		}
		prevPlayerTile = playerTile;
	}
}


void Enemy::Update(double dt, Player* thePlayer, Player* otherPlay, CMap* m_cMap)
{
    if (m_hp <= 0)
        m_active = false;

    Player* temp = thePlayer;

    findTargetDelay -= dt;
    if (findTargetDelay < 0)
    {
        if ((thePlayer->GetPos() - m_pos).LengthSquared() >= (otherPlay->GetPos() - m_pos).LengthSquared())
            temp = thePlayer;
        else
            temp = otherPlay;

        findTargetDelay = 1;
    }
        MoveToPlayer(dt, temp, m_cMap);

    //Vector3 dir = (m_target - this->m_pos).Normalized();
    //m_pos = m_pos + dir * 0.3;

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
    if (m_attackDelay <= 0)
    {
        Attack();
        m_attackDelay = 0.5;
    }
    
}

void Enemy::SetHP(int hp)
{
    m_hp = hp;
}

int Enemy::GetHp()
{
    return m_hp;
}

void Enemy::SetTarget(Vector3 target)
{
    m_target = target;
}

Vector3 Enemy::GetTarget()
{
    return m_target;
}

void Enemy::SetEnemyType(ENEMY_TYPE type)
{
    m_enemyType = type;
}

Enemy::ENEMY_TYPE Enemy::GetEnemyType()
{
    return m_enemyType;
}

void Enemy::Attack()
{
    switch (m_enemyType)
    {
    case MELEE:
    {
                  Projectile* temp = new Projectile();
                  temp->SetActive(true);
                  temp->SetDmg(1);
                  temp->SetLifetime(0.3);
                  temp->SetPos(this->m_pos);
                  temp->SetScale(Vector3(5, 5, 1));
                  temp->SetType(GameObject::PROJECTILE_MELEE);

                  Vector3 dir = (m_target - this->m_pos).Normalized();
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
                   temp->SetLifetime(1);
                   temp->SetPos(this->m_pos);
                   temp->SetScale(Vector3(5, 5, 1));
                   temp->SetType(GameObject::PROJECTILE_RANGED);

                   Vector3 dir = (m_target - this->m_pos).Normalized();
                   temp->SetVelocity(dir);

                   m_ProjectileList.push_back(temp);
                   break;
    }
    }
}

void Enemy::TakeDamage(int dmg)
{
    m_hp -= dmg;
}