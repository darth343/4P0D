#include "Player.h"
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
}