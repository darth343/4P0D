#ifndef ASSIGNMENT1_H
#define ASSIGNMENT1_H

#include "GameObject.h"
#include "SceneBase.h"
#include "WaveData.h"
#include <vector>
#include <irrKlang.h>

using namespace irrklang;

class SceneAss1 : public SceneBase
{
    enum GAMEPLAY_STATE
    {
        GAMEPLAY_PLAY,
        GAMEPLAY_PAUSE,
        GAMEPLAY_WINLOSE,
        STATE_TOTAL
    };

    const int MAX_SPEED;
    const int BULLET_SPEED;
    const int MISSILE_SPEED;
    const int E_BULLET_SPEED;

public:
    SceneAss1();
    ~SceneAss1();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

    // Spawn
    void SpawnAsteroid(float posX, float posY, int hp = 1);
    void SpawnMeteor(float posX, float posY);
    void SpawnEnemyShip(GameObject::ENEMY_TYPE type);
    void SpawnEnemyBullet();
    void SpawnBullet();
    void SpawnMissile();
    void SpawnBlackHole();
    bool CalculateDropRate(const double dt);
    void SpawnDrop(float posX, float posY);
    void SpawnWave(const double dt);

    // Collision
    bool CheckCollision(GameObject *go, GameObject *go2);
    void PlayerGetHit();
    void ResetPlayer();

    // Update
    void PauseUpdate(double dt);
    void GameUpdate(double dt);
    void LevelTransition(double dt);

    void UpdatePlayerShip(const double dt);
    void UpdateBlackhole(const double dt);
    void ExplodeBlackhole();
    bool CheckCollisionWithBlackhole(GameObject* go);

    void UpdateEnemyShip(const double dt);
    void UpdateAsteroid(GameObject* go, const double dt);
    void UpdateMeteor(GameObject* go, const double dt);
    void UpdateBullet(GameObject* go, const double dt);
    void UpdateEnemyBullet(GameObject* go, const double dt);
    void UpdateMissile(GameObject* go, const double dt);
    void UpdateDrop(GameObject* go, const double dt);

    // Render
    void RenderPause();
    void RenderLevelTransition();

    void RenderGO(GameObject *go);
    void RenderBackground();
    void RenderInfoOnScreen();
    void RenderBlackHole(float posX, float posY, float angle = 0.f, bool ghost = true);

    GameObject* FetchGO();
protected:
    float backgroundOffset;   //for background

    //Level Data
    GAMEPLAY_STATE state;
    int levelCount;
    bool b_boss;

    std::vector<WaveData*> m_levelData;
    std::vector<WaveData*>::iterator m_itLevelData;
    double waveTimeElapsed;
    bool b_endOfWave;

    //Game Objects
    std::vector<GameObject *> m_goList;
    std::vector<GameObject*>::iterator m_itFetchGO;
    float m_renderCount;
    bool b_spawnSmallAsteroids;
    Vector3 asteroidPos;

    //Physics variables
    float m_worldWidth;
    float m_worldHeight;
    GameObject *m_ship;
    Vector3 m_gravity;
    int m_objectCount;

    // Game variables related to drops
    int m_lives;
    int m_score;
    int m_bullets;
    int missile_count;
    int blackhole_count;

    // black hole skill
    GameObject *m_blackhole;
    bool b_blackhole_activated; //LMB to put down, RMB to cancel skill
    double blackhole_timeElapsed;   //how long the black hole has been active
    double blackhole_lifetime;  //how long the black hole lasts for
    ISound* SE_blackhole;

    // enemy
    GameObject* m_enemyShip;

    //timers
    double bulletTimeElapsed;
    double bulletSpawnRate;

    double missileTimeElapsed;
    double missileSpawnRate;

    double dropTimeElapsed;
    double dropSpawnRate;

    double deathTimeElapsed;
    double invulnerabilityTimeElapsed;

    // Pause screen
    bool button_highlighted[2];
    bool bLButtonState;
};

#endif