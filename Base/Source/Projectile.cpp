#include "Projectile.h"

Projectile::Projectile()
{}

Projectile::~Projectile()
{}

void Projectile::Init()
{

}

void Projectile::Update(double dt)
{
    m_pos = m_pos + m_velocity;
}

void Projectile::SetDmg(int dmg)
{
    m_dmg = dmg;
}

int Projectile::GetDmg()
{
    return m_dmg;
}