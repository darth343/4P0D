#pragma once
#include "Map.h"
#include "Mesh.h"

class Player
{
public:
	Player(void);
	~Player(void);

	enum GEOMETRY_TYPE
	{
		GEO_TILEHERO_FRAME0,
		GEO_TILEHERO_FRAME1,
		GEO_TILEHERO_FRAME2,
		GEO_TILEHERO_FRAME3,
		NUM_GEOMETRY,
	};

	// Initialise this class instance
	void Init(void);
	// Update Movements
	void MoveUpDown(const bool mode, CMap* m_cMap, double dt);
	void MoveLeftRight(const bool mode, CMap* m_cMap, double dt);
	// Get Position
	Vector3 GetPosition();
	// Set Position
	void SetPosition(Vector3 pos);
	// Get mapOffset
	Vector3 GetMapOffset(void);
	// Set mapOffset
	void SetMapOffset(Vector3 MapOffset);
	// Hero Update
	void HeroUpdate(CMap* m_cMap, double dt);
	// Get Player Mesh
	Mesh* GetPlayerMesh();
	// Add Player HP
	void AddHealth(int health);
	// Set Player Mesh
	void SetPlayerMesh(Mesh* mesh);
	// Get Flip Status
	bool GetFlipStatus();
	// Set Flip Status
	void SetFlipStatus(bool flip);
	// Constrain the position of the Hero to within the border
	void ConstrainHero(const int leftBorder, const int rightBorder, 
					   const int topBorder, const int bottomBorder, 
					   double dt, CMap* m_cMap, bool constrainX, bool constrainY);

	void SetHP(float hp);	
	void SetMP(float mp);
	void SetDMG(float dmg);	
	void SetDEF(float def);
	void TakeDMG(float dmg);
	void UseMP(float mp);
	void Reset();

	float GetHP();
	float GetMP();
	float GetDMG();
	float GetDEF();
	int controllerID;
private:
	// Hero's information
	Vector3 theHeroPosition;
	bool FlipStatus;
	Mesh* playerMesh;
	// For scrolling.
	static Vector3 mapOffset;
	float playerHP;
	float playerMP;
	float playerDamage;
	float playerDefense;
};
