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