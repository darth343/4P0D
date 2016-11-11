#include "Player.h"
#include <iostream>
#include "Application.h"

Vector3 Player::mapOffset = Vector3(0,0,0);

Player::Player(void)
	: playerMesh(NULL)
	, FlipStatus(false)
	, playerHP(100)
	, playerMP(100)
	, playerDamage(20)
	, playerDefense(0)
{
}

const float MOVEMENT_SPEED = 2.f;
const float xOffset = 8.f;
const float yOffset = 2.f;

Player::~Player(void)
{
}

void Player::Reset()
{
	this->playerHP = 100;
	this->playerMP = 100;
}

// Initialise this class instance
void Player::Init(void)
{
	theHeroPosition.x=0;
	theHeroPosition.y=0;

}

void Player::AddHealth(int health)
{
	playerHP += health;
	if (playerHP > 100)
		playerHP = 100;
}

Mesh* Player::GetPlayerMesh()
{
	return playerMesh;
}

bool Player::GetFlipStatus()
{
	return FlipStatus;
}

void Player::SetPlayerMesh(Mesh* mesh)
{
	this->playerMesh = mesh;
}

void Player::TakeDMG(float dmg)
{
	this->SetHP(this->playerHP - dmg);
}

void Player::UseMP(float mp)
{
	this->SetMP(playerMP - mp);
}

/********************************************************************************
 Hero Move Up Down
 ********************************************************************************/
void Player::MoveUpDown(const bool mode, CMap* m_cMap, double dt)
{
	if (mode)
	{
		Vector3 nextPos = theHeroPosition;
		nextPos.y = theHeroPosition.y - (MOVEMENT_SPEED * dt);
		Vector3 checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
		if (checkPosition.y < 0)
		{
			checkPosition.y = 0;
		}
		if (checkPosition.y + 1 > m_cMap->GetNumOfTiles_Height())
		{
			checkPosition.y = m_cMap->GetNumOfTiles_Height();
		}
		if (fmod(theHeroPosition.x + mapOffset.x, m_cMap->GetTileSize()) < 0.2)
		{
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].shouldCollide)
			{
				theHeroPosition.y = nextPos.y;
			}
			else
			{
				theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
			}
		}
		else
		{
			nextPos.y = theHeroPosition.y - (MOVEMENT_SPEED * dt);
			nextPos.x += xOffset;
			checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].shouldCollide)
			{
				nextPos.y = theHeroPosition.y - (MOVEMENT_SPEED * dt);
				nextPos.x -= xOffset;
				checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
				if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].shouldCollide)
				{
					theHeroPosition.y = nextPos.y;
				}
				else
				{
					theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
				}
			}
			else
			{
				theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
			}
		}
	}
	else
	{
		Vector3 nextPos = theHeroPosition;
		nextPos.y = theHeroPosition.y + (MOVEMENT_SPEED * dt);
		Vector3 checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
		if (checkPosition.y < 0)
		{
			checkPosition.y = 0;
		}
		if (checkPosition.y + 1 > m_cMap->GetNumOfTiles_Height())
		{
			checkPosition.y = m_cMap->GetNumOfTiles_Height();
		}
		if (fmod(theHeroPosition.x + mapOffset.x, m_cMap->GetTileSize()) < 0.2)
		{
			if (!m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].shouldCollide)
			{
				theHeroPosition.y = nextPos.y;
			}
			else
			{
				theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
			}
		}
		else
		{
			nextPos.y = theHeroPosition.y + (MOVEMENT_SPEED * dt);
			nextPos.x += xOffset;
			checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
			if (!m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].shouldCollide)
			{
				nextPos.y = theHeroPosition.y + (MOVEMENT_SPEED * dt);
				nextPos.x -= xOffset;
				checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
				if (!m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x + 1].shouldCollide)
				{
					theHeroPosition.y = nextPos.y;
				}
				else
				{
					theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
				}
			}
			else
			{
				theHeroPosition.y = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).y - mapOffset.y;
			}
		}
	}
}

/********************************************************************************
 Hero Move Left Right
 ********************************************************************************/
void Player::MoveLeftRight(const bool mode, CMap* m_cMap, double dt)
{
	if (mode)
	{
		Vector3 nextPos = theHeroPosition;
		nextPos.x = theHeroPosition.x - (MOVEMENT_SPEED * dt);
		Vector3 checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
		if (checkPosition.x < 0)
		{
			checkPosition.x = 0;
		}
		if (checkPosition.x > m_cMap->GetNumOfTiles_Width())
		{
			checkPosition.x = m_cMap->GetNumOfTiles_Width();
		}
		if (fmod(theHeroPosition.y + mapOffset.y, m_cMap->GetTileSize()) < 0.2)
		{
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].shouldCollide)
			{
				theHeroPosition.x = nextPos.x;
			}
			else
			{
				theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
			}
		}
		else{
			nextPos = theHeroPosition;
			nextPos.x = theHeroPosition.x - (MOVEMENT_SPEED * dt);
			nextPos.y += yOffset;
			checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].shouldCollide)
			{
				nextPos = theHeroPosition;
				nextPos.x = theHeroPosition.x - (MOVEMENT_SPEED * dt);
				nextPos.y -= yOffset;
				checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
				if (!m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x].shouldCollide)
				{
					theHeroPosition.x = nextPos.x;
				}
				else
				{
					theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
				}

			}
			else
			{
				theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
			}
		}
	}else
	{
		Vector3 nextPos = theHeroPosition;
		nextPos.x = theHeroPosition.x + (MOVEMENT_SPEED * dt);
		Vector3 checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
		if (checkPosition.x < 0)
		{
			checkPosition.x = 0;
		}
		if (checkPosition.x+1 > m_cMap->GetNumOfTiles_Width())
		{
			checkPosition.x = m_cMap->GetNumOfTiles_Width();
		}
		if (fmod(theHeroPosition.y + mapOffset.y, m_cMap->GetTileSize()) < 0.1)
		{
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].shouldCollide)
			{
				theHeroPosition.x = nextPos.x;
			}
			else
			{
				theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
			}
		}
		else
		{
			nextPos = theHeroPosition;
			nextPos.x = theHeroPosition.x + (MOVEMENT_SPEED * dt);
			nextPos.y += yOffset;
			checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
			if (!m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x + 1].shouldCollide)
			{
				nextPos = theHeroPosition;
				nextPos.x = theHeroPosition.x + (MOVEMENT_SPEED * dt);
				nextPos.y -= yOffset;
				checkPosition = (nextPos + mapOffset) * (1.f / m_cMap->GetTileSize());
				if (!m_cMap->theMap[(int)checkPosition.y + 1][(int)checkPosition.x + 1].shouldCollide)
				{
					theHeroPosition.x = nextPos.x;
				}
				else
				{
					theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
				}
			}
			else
			{
				theHeroPosition.x = (m_cMap->theMap[(int)checkPosition.y][(int)checkPosition.x].Pos * m_cMap->GetTileSize()).x - mapOffset.x;
			}
		}
	}
}

Vector3 Player::GetMapOffset(void)
{
	return mapOffset;
}

void Player::SetMapOffset(Vector3 MapOffset)
{
	mapOffset = MapOffset;
}

void Player::SetFlipStatus(bool flip)
{
	FlipStatus = flip;
}

Vector3 Player::GetPosition()
{
	return theHeroPosition;
}

void Player::SetPosition(Vector3 pos)
{
	theHeroPosition = pos;
}

// Constrain the position of the Hero to within the border
void Player::ConstrainHero(const int leftBorder, const int rightBorder,
	const int topBorder, const int bottomBorder,
	double dt, CMap* m_cMap, bool constrainX, bool constrainY)
{
	if (constrainX)
	{
		if (mapOffset.x != 0)
		{
			if (theHeroPosition.x < leftBorder)
			{
				float displacement = leftBorder - theHeroPosition.x;
				mapOffset.x -= displacement;
				theHeroPosition.x = leftBorder;
				//mapOffset.x = mapOffset.x - (int)(CONSTRAINTSPEED * dt);
				if (mapOffset.x < 0)
					mapOffset.x = 0;
			}
		}
		if (mapOffset.x < m_cMap->GetNumOfTiles_Width() * m_cMap->GetTileSize() - 800)
		{
			if (mapOffset.x < m_cMap->getScreenWidth() - 1.f)
			{
				if (theHeroPosition.x > rightBorder)
				{
					float displacement = theHeroPosition.x - rightBorder;
					mapOffset.x += displacement;
					theHeroPosition.x = rightBorder;
					//mapOffset.x = mapOffset.x + (int)(CONSTRAINTSPEED * dt);
					if (mapOffset.x > m_cMap->theScreen_Width)	// This must be changed to soft-coded
						mapOffset.x = m_cMap->theScreen_Width;
				}
			}
		}
		else
		{
			mapOffset.x = m_cMap->GetNumOfTiles_Width() * m_cMap->GetTileSize() - 800;
		}
	}
	else
	{
		if (theHeroPosition.x > 800 - m_cMap->GetTileSize())
		{
			theHeroPosition.x = 800 - m_cMap->GetTileSize();
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

	if (constrainY)
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
	//playerDamage = Equip.getEQAtk();
	return playerDamage;
}

float Player::GetDEF()
{
	//playerDefense = Equip.getEQDef();
	return playerDefense;
}

float Player::GetMP()
{
	return playerMP;
}

