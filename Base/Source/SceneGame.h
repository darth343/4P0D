#ifndef SCENE_GAME_H
#define SCENE_GAME_H

#include "GameObject.h"
#include "SceneBase.h"
#include <vector>
#include <irrKlang.h>

using namespace irrklang;

class SceneGame : public SceneBase
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
    SceneGame();
    ~SceneGame();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

    // Collision
    bool CheckCollision(GameObject *go, GameObject *go2);
    void PlayerGetHit();
    void ResetPlayer();

    // Update
    void PauseUpdate(double dt);
    void GameUpdate(double dt);

    // Render
    void RenderPause();
    void RenderLevelTransition();

    void RenderGO(GameObject *go);
    void RenderBackground();
    void RenderPlayer();
    void RenderInfoOnScreen();

    GameObject* FetchGO();
protected:
    float backgroundOffset;   //for background

    //Level Data
    GAMEPLAY_STATE state;
    int levelCount;
    bool b_boss;

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

    // Pause screen
    bool button_highlighted[2];
    bool bLButtonState;
};

#endif