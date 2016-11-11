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

    m_lifetime -= dt;

    if (m_lifetime <= 0)
        m_active = false;
}

void Projectile::SetDmg(int dmg)
{
    m_dmg = dmg;
}

int Projectile::GetDmg()
{
    return m_dmg;
}

void Projectile::SetLifetime(double lifetime)
{
    m_lifetime = lifetime;
}

double Projectile::GetLifetime()
{
    return m_lifetime;
}