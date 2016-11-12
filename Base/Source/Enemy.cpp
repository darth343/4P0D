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
{
    if (m_Emitter)
        delete m_Emitter;
}

void Enemy::Init()
{
    m_Emitter = new Emitter();
    m_Emitter->m_lifetime = 0.15;
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
    m_Emitter->SetEmitterPosition(m_pos);
    if (m_enemyType == MELEE)
        m_Emitter->SetParticleType(ParticleObject_TYPE::P_NORMAL_BLOOD);
    else
        m_Emitter->SetParticleType(ParticleObject_TYPE::P_SILVER_BLOOD);

    m_Emitter->SetMaxParticle(10);
    m_Emitter->SetSpawnLocation(m_pos - Vector3(10, 10, 1), m_pos + Vector3(10, 10, 1));  
    m_Emitter->SetParticleSpawnRate(3);


    if (m_hp <= 0)
    {
        if (m_Emitter->m_lifetime >= 0)
        {
            m_Emitter->SpawnParticle();
            std::vector<ParticleObject*>tempVec = m_Emitter->GetParticles();

            std::vector<ParticleObject*>::iterator it;
            std::vector<ParticleObject*>::iterator end = tempVec.end();
            for (it = tempVec.begin(); it != end; ++it)
            {
                ParticleObject* particle = (ParticleObject*)*it;

                particle->m_lifetime -= dt;

                if (particle->m_lifetime <= 0)
                {
                    particle = false;
                    continue;
                }
                if (particle->active)
                {
                    switch (particle->type)
                    {
                    case P_NORMAL_BLOOD:
                    {
                                           particle->pos += particle->vel * (float)dt * 100.f;
                                           particle->rotation += particle->rotationSpeed * (float)dt;
                                           break;
                    }
                    case P_GREEN_BLOOD:
                    {
                                          particle->pos += particle->vel * (float)dt * 100.f;
                                          particle->rotation += particle->rotationSpeed * (float)dt;
                                          break;
                    }

                    case P_SILVER_BLOOD:
                    {
                                           particle->pos += particle->vel * (float)dt * 100.f;
                                           particle->rotation += particle->rotationSpeed * (float)dt;
                                           break;
                    }

                    }
                }
            }
            m_Emitter->m_lifetime -= dt;
        }
        else
        {
            m_active = false;
        }
    }
    Player* temp = NULL;

    findTargetDelay -= dt;

    if (((thePlayer->GetPos() - m_pos).Length() < 250) && ((otherPlay->GetPos() - m_pos).Length() > 250))
    {
        temp = thePlayer;
    }
    else if (((thePlayer->GetPos() - m_pos).Length() > 250) && ((otherPlay->GetPos() - m_pos).Length() < 250))
    {
        temp = otherPlay;
    }
    else if (((thePlayer->GetPos() - m_pos).Length() < 250) && ((otherPlay->GetPos() - m_pos).Length() < 250))
    {
        if ((thePlayer->GetPos() - m_pos).LengthSquared() >= (otherPlay->GetPos() - m_pos).LengthSquared())
            temp = thePlayer;
        else
            temp = otherPlay;

        findTargetDelay = 1;
    }

    if (temp)
    {
        m_target = temp->GetPos();
        MoveToPlayer(dt, temp, m_cMap);
    }

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

                  if (m_target == this->m_pos)
                      break;

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

                   if (m_target == this->m_pos)
                       break;

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