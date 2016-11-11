#include "Enemy.h"

Enemy::Enemy()
: m_attackDelay(0)
, m_hp(1)
{}

Enemy::~Enemy()
{}

void Enemy::Init()
{
    
}

void Enemy::Update(double dt, Vector3 newTarget)
{
    if (m_hp <= 0)
        m_active = false;

    if (newTarget != m_target)
        m_target = newTarget;

    Vector3 dir = (m_target - this->m_pos).Normalized();
    m_pos = m_pos + dir * 0.3;

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
                  temp->SetLifetime(0.1);
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
                   temp->SetLifetime(0.5);
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