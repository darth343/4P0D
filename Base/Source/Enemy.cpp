#include "Enemy.h"

Enemy::Enemy()
{}

Enemy::~Enemy()
{}

void Enemy::Init()
{
    
}

void Enemy::Update(double dt)
{
    m_pos = m_pos + m_velocity;
}

void Enemy::SetHP(int hp)
{
    m_hp = hp;
}

int Enemy::GetHp()
{
    return m_hp;
}