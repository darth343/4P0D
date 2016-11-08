#include "SceneAss1.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAss1::SceneAss1() : MAX_SPEED(1), BULLET_SPEED(100), MISSILE_SPEED(20), E_BULLET_SPEED(50)
{
}

SceneAss1::~SceneAss1()
{
}

void SceneAss1::Init()
{
    SceneBase::Init();

    Math::InitRNG();

    // World Coordinates
    m_worldHeight = 100.f;
    m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

    // Construct 10 GameObject with type GO_ASTEROID and add into m_goList
    for (unsigned i = 0; i < 10; ++i) {
        m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
    }
    m_itFetchGO = m_goList.begin();
    m_renderCount = 0.f;

    // Initialize game variables
    m_lives = 3;
    m_score = 0;
    m_bullets = 1;
    missile_count = 10;
    blackhole_count = 3;

    m_gravity.Set(0, -4.9f, 0);

    // Black hole skill
    m_blackhole = new GameObject(GameObject::GO_BLACKHOLE);
    m_blackhole->active = false;
    m_blackhole->scale.Set(15.f, 15.f, 15.f);
    m_blackhole->angle = 0.f;
    m_blackhole->rotSpeed = 20.f;
    m_blackhole->vel.Set(-10.f, 0, 0);   //to move it across the screen
    
    b_blackhole_activated = false;
    blackhole_timeElapsed = 0.0;
    blackhole_lifetime = 5.0;
    SE_blackhole = 0;

    // Construct m_ship, set active, type, scale and pos
    m_ship = new GameObject(GameObject::GO_SHIP);
    m_ship->active = true;
    m_ship->scale.Set(4.f, 4.f, 4.f);
    m_ship->force.SetZero();
    m_ship->mass = 10.f;
    ResetPlayer();

    m_enemyShip = new GameObject(GameObject::GO_ENEMY_SHIP);
    m_enemyShip->active = false;

    //timers
    bulletTimeElapsed = 0.0;
    bulletSpawnRate = 0.2;

    missileTimeElapsed = 1.0;
    missileSpawnRate = 1.0;

    dropTimeElapsed = 0.0;
    dropSpawnRate = 1.5;

    deathTimeElapsed = 0.0;
    invulnerabilityTimeElapsed = 0.0;

    //background
    backgroundOffset = 0.f;     //width offset

    //Level wave
    state = GAMEPLAY_PLAY;
    levelCount = 1;
    b_boss = false;
    LoadLevel("Level/level1.txt", m_levelData);
    
    m_itLevelData = m_levelData.begin();
    waveTimeElapsed = 0.0;
    b_endOfWave = false;

    b_spawnSmallAsteroids = false;

    // Pause screen
    button_highlighted[0] = false;
    button_highlighted[1] = false;
    bLButtonState = false;
}

GameObject* SceneAss1::FetchGO()
{
    // OPTIMISE BY SAVING THE ITERATOR

    for (std::vector<GameObject*>::iterator it = m_itFetchGO; (m_itFetchGO == m_goList.begin()) ? (it != m_goList.end()) : (it != m_itFetchGO - 1); (it == m_goList.end()) ? (it = m_goList.begin()) : (++it)) {
        if (it == m_goList.end()) {
            continue;
        }
        GameObject *go = (GameObject *)*it;
        if (!go->active) {      //object is inactive
            go->active = true;
            m_itFetchGO = it;
            ++GameObject::GO_COUNT;
            return go;
        }

    }

    //m_goList ran out of objects
    for (unsigned i = 0; i < 10; ++i) {
        m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
    }
    GameObject *go = (GameObject *)*(m_goList.end() - 1);
    go->active = true;
    m_itFetchGO = m_goList.begin();
    ++GameObject::GO_COUNT;
    return go;
}

void SceneAss1::UpdatePlayerShip(const double dt)
{
    // Move the ship every frame
    Vector3 acceleration;   //default Vector3 is set to (0, 0, 0)
    if (m_ship->mass > Math::EPSILON) {
        acceleration = m_ship->force * (1.f / m_ship->mass);
    }
    m_ship->vel += acceleration * dt;

    if (m_ship->vel.LengthSquared() > MAX_SPEED * MAX_SPEED) {  //at max velocity
        m_ship->vel.Normalize();
        m_ship->vel *= MAX_SPEED;
    }

    // Set boundaries for the ship
    if (m_ship->pos.x > m_worldWidth + 1) {
        //m_ship->pos.x = m_worldWidth + 1;
        m_ship->force.Set(-100.f, 0, 0);
    }
    else if (m_ship->pos.x < -1) {
        //m_ship->pos.x = -1;
        m_ship->force.Set(100.f, 0, 0);
    }
    else if (m_ship->pos.y > m_worldHeight + 1) {
        //m_ship->pos.y = m_worldHeight + 1;
        m_ship->force.Set(0, -100.f, 0);
    }
    else if (m_ship->pos.y < -1) {
        //m_ship->pos.y = -1;
        m_ship->force.Set(0, 100.f, 0);
    }
    
    Vector3 acceleration2;   //default Vector3 is set to (0, 0, 0)
    if (m_ship->mass > Math::EPSILON) {
        acceleration2 = m_ship->force * (1.f / m_ship->mass);
    }
    m_ship->vel += acceleration2 * dt;
    
    m_ship->pos += m_ship->vel;

    //slow the ship down every frame
    if (m_ship->vel.LengthSquared() >= Math::EPSILON) {

        Vector3 deceleration;   //default Vector3 is set to (0, 0, 0)
        if (m_ship->mass > Math::EPSILON) {
            deceleration = m_ship->vel.Normalized() * (1.f / m_ship->mass);
        }
        m_ship->vel -= deceleration * dt;
    }

    // Affect the ship by gravity
    m_ship->pos += m_gravity * dt;
}

void SceneAss1::UpdateBlackhole(const double dt)
{
    m_blackhole->pos += m_blackhole->vel * dt;
    m_blackhole->angle += m_blackhole->rotSpeed * dt;

    blackhole_timeElapsed += dt;

    // Unspawn blackhole when its time is up
    if (blackhole_timeElapsed > blackhole_lifetime) {
        m_blackhole->active = false;
        blackhole_timeElapsed = 0.0;
        if (SE_blackhole) {
            SE_blackhole->stop();
        }
        ExplodeBlackhole();
    }

    // Unspawn blackhole if it moves out of screen
    if (m_blackhole->pos.x < -2.f * m_blackhole->scale.x) {
        m_blackhole->active = false;
        blackhole_timeElapsed = 0.0;
        if (SE_blackhole) {
            SE_blackhole->stop();
        }
        ExplodeBlackhole();
    }
}

void SceneAss1::ExplodeBlackhole()
{
    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->active) {
            if (CheckCollision(go, m_blackhole)) {
                go->SetInactive();
                m_score += 50;      //score for destroying objects
            }
        }
    }
}

bool SceneAss1::CheckCollisionWithBlackhole(GameObject* go)
{
    float distSquare = (go->pos - m_blackhole->pos).LengthSquared();
    float combinedRadiusSquare = (go->scale.x + 2.f * m_blackhole->scale.x) * (go->scale.y + 2.f * m_blackhole->scale.y);

    if (distSquare <= combinedRadiusSquare) {
        return true;
    }
    return false;
}

void SceneAss1::UpdateAsteroid(GameObject* go, const double dt)
{
    if (m_blackhole->active) {
        if (CheckCollisionWithBlackhole(go)) {
            go->vel = (go->scale.x + 3.f * m_blackhole->scale.x - (m_blackhole->pos - go->pos).Length()) * (m_blackhole->pos - go->pos).Normalized();
        }
    }

    // MOVE THE ASTEROID
    go->vel += m_gravity * dt;
    go->pos += go->vel * dt;

    //rotate asteroid
    go->angle += go->rotSpeed * dt;
    if (go->angle > 360.f) {
        go->angle -= 360.f;
    }

    // Handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
    if (m_ship->active && CheckCollision(go, m_ship)) {    //there is collision
        PlayerGetHit();
        go->SetInactive();      //remove the asteroid

        if (go->hp == 2) {    //big asteroid destroyed
            b_spawnSmallAsteroids = true;
            asteroidPos = go->pos;
        }
    }

    // Unspawn when they leave screen
    if (go->active) {
        if (go->pos.x < -go->scale.x || go->pos.y < -go->scale.x) {
            go->SetInactive();
        }
    }
}

void SceneAss1::UpdateMeteor(GameObject* go, const double dt)
{
    if (m_blackhole->active) {
        if (CheckCollisionWithBlackhole(go)) {
            go->vel = (go->scale.x + 3.f * m_blackhole->scale.x - (m_blackhole->pos - go->pos).Length()) * (m_blackhole->pos - go->pos).Normalized();
        }
    }

    // MOVE THE METEOR
    go->vel += m_gravity * dt;
    go->pos += go->vel * dt;

    //Check for collision with player's ship
    if (m_ship->active && CheckCollision(go, m_ship)) {    //there is collision
        PlayerGetHit();
        go->SetInactive();     //remove the meteor
    }

    // Unspawn when they leave screen
    if (go->active) {
        if (go->pos.x < -go->scale.x || go->pos.y > m_worldHeight + go->scale.x || go->pos.y < -go->scale.x) {
            go->SetInactive();
        }
    }
}

void SceneAss1::UpdateEnemyShip(const double dt)
{
    if (m_enemyShip->bosstype == GameObject::ENEMY_BOSS1)
    {
        if (m_blackhole->active && CheckCollisionWithBlackhole(m_enemyShip)) {
            m_enemyShip->vel = (m_enemyShip->scale.x + 3.f * m_blackhole->scale.x - (m_blackhole->pos - m_enemyShip->pos).Length()) * (m_blackhole->pos - m_enemyShip->pos).Normalized();
        }
        else {
            // MOVE THE ENEMY SHIP
            //go->vel = go->vel.Length() * Vector3(go->target->pos - go->pos).Normalized();
            Vector3 acceleration;
            m_enemyShip->direction = Vector3(m_ship->pos - m_enemyShip->pos).Normalized();
            float magnitude = Vector3(m_ship->pos - m_enemyShip->pos).Length();  //of force
            m_enemyShip->force = magnitude * m_enemyShip->direction;
            acceleration = m_enemyShip->force * (1.f / m_enemyShip->mass);
            m_enemyShip->vel += acceleration * dt;
            m_enemyShip->vel = m_enemyShip->vel.Normalized() * 20.f;
        }

        // MOVE THE ENEMY SHIP
        //go->vel += m_gravity * dt;
        m_enemyShip->pos += m_enemyShip->vel * dt;

        // Shoot bullet
        m_enemyShip->shootTimeElapsed += dt;
        if (m_ship->active && m_enemyShip->shootTimeElapsed > m_enemyShip->shootRate) {
            SpawnEnemyBullet();
            m_enemyShip->shootTimeElapsed = 0.0;
        }
        else if (!m_ship->active) {
            m_enemyShip->shootTimeElapsed = 0.0;
        }
    }

    else if (m_enemyShip->bosstype == GameObject::ENEMY_BOSS2)
    {
        // MOVE THE ENEMY SHIP
        if (m_enemyShip->vel.LengthSquared() >= Math::EPSILON) {

            Vector3 deceleration;   //default Vector3 is set to (0, 0, 0)
            if (m_enemyShip->mass > Math::EPSILON) {
                deceleration = m_enemyShip->vel.Normalized() * (1.f / m_enemyShip->mass);
            }
            m_enemyShip->vel -= deceleration * dt;
        }
        m_enemyShip->pos += m_enemyShip->vel * dt;

        // Change enemy ship direction
        m_enemyShip->direction = Vector3(m_ship->pos - m_enemyShip->pos).Normalized();

        // Shoot bullet
        m_enemyShip->shootTimeElapsed += dt;
        if (m_ship->active && m_enemyShip->shootTimeElapsed > m_enemyShip->shootRate) {
            SpawnEnemyBullet();
            m_enemyShip->shootTimeElapsed = 0.0;
        }
        else if (!m_ship->active) {
            m_enemyShip->shootTimeElapsed = 0.0;
        }
    }

}

void SceneAss1::UpdateBullet(GameObject* go, const double dt)
{
    // MOVE THE BULLET
    go->vel += 2.f * m_gravity * dt;
    go->pos += go->vel * dt;

    // Collision check between GO_BULLET and GO_ASTEROID
    for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
    {
        GameObject *go2 = (GameObject *)*it2;
        if (go2->active) {
            if (go2->type == GameObject::GO_ASTEROID) {     //check for collision with asteroid

                if (CheckCollision(go, go2)) {    //there is collision
                    go->SetInactive();     //bullet is deleted
                    go2->SetInactive();    //asteroid is deleted
                    if (go2->hp == 1) {     //small asteroid destroyed
                        m_score += 100;
                        if (CalculateDropRate(dt)) {  //spawn a drop
                            SpawnDrop(go->pos.x, go->pos.y);
                        }
                    }
                    else if (go2->hp == 2) {    //big asteroid destroyed
                        b_spawnSmallAsteroids = true;
                        asteroidPos = go2->pos;
                    }
                    break;
                }

            }

        }

    }

    // Collision with enemy ship
    if (m_enemyShip->active) {  //check for collision with enemy ship

        if (CheckCollision(go, m_enemyShip)) {    //there is collision
            go->SetInactive();     //bullet is deleted
            --m_enemyShip->hp;
            if (m_enemyShip->hp == 0) {
                m_enemyShip->active = false;
                m_score += 500;
                b_boss = false;
            }
        }
    }

    // Unspawn bullets when they leave screen
    if (go->active) {
        if (go->pos.x > m_worldWidth + go->scale.x || go->pos.x < -go->scale.x || go->pos.y > m_worldHeight + go->scale.x || go->pos.y < -go->scale.x) {
            go->SetInactive();
        }
    }
}

void SceneAss1::UpdateEnemyBullet(GameObject* go, const double dt)
{
    // MOVE THE BULLET
    go->vel += 2.f * m_gravity * dt;
    go->pos += go->vel * dt;

    // Collision check between GO_ENEMY_BULLET and player ship
    if (m_ship->active) {
        if (CheckCollision(go, m_ship)) {    //there is collision
            go->SetInactive();     //bullet is deleted
            PlayerGetHit();
        }
    }

    // Unspawn bullets when they leave screen
    if (go->active) {
        if (go->pos.x > m_worldWidth + go->scale.x || go->pos.x < -go->scale.x || go->pos.y > m_worldHeight + go->scale.x || go->pos.y < -go->scale.x) {
            go->SetInactive();
        }
    }
}

void SceneAss1::UpdateMissile(GameObject* go, const double dt)
{
    if (go->target != 0 && !go->target->active) {   //target has gone inactive
        go->target = 0;
    }
    // MOVE THE MISSILE
    go->vel += 2.f * m_gravity * dt;

    // Change velocity based on target
    if (go->target != 0 && go->target->active) {  //there is a target
        //go->vel = go->vel.Length() * Vector3(go->target->pos - go->pos).Normalized();
        Vector3 acceleration;
        go->force = (Vector3(m_worldWidth, m_worldHeight, 0) - Vector3(go->target->pos - go->pos)).Length() * Vector3(go->target->pos - go->pos).Normalized();
        acceleration = go->force * (1.f / go->mass);
        go->vel += acceleration * dt;
        go->vel = go->vel.Normalized() * (Vector3(m_worldWidth, m_worldHeight, 0) - Vector3(go->target->pos - go->pos)).Length() * 0.25f;    //change velocity speed based on distance to target
    }

    go->pos += go->vel * dt;

    // Collision check between GO_MISSILE and target

    if (go->target != 0 && go->target->active) {
        if (CheckCollision(go, go->target)) {
            if (go->target->type == GameObject::GO_ASTEROID) {     //target is asteroid
                go->SetInactive();     //missile is deleted
                go->target->SetInactive();    //target is deleted
                m_score += 50;
                if (CalculateDropRate(dt)) {  //spawn a drop
                    SpawnDrop(go->pos.x, go->pos.y);
                }
            }
            else if (go->target->type == GameObject::GO_METEOR) {

                go->SetInactive();     //bullet is deleted
                go->target->SetInactive();    //asteroid is deleted
                m_score += 50;
            }
            else if (go->target->type == GameObject::GO_ENEMY_SHIP) {  //check for collision with enemy ship

                go->SetInactive();     //bullet is deleted
                --go->target->hp;
                if (go->target->hp == 0) {
                    go->target->active = false;
                    m_score += 500;
                    b_boss = false;
                }

            }
        }

    }

    // Unspawn missile when it leaves screen & target is gone
    if (go->active && go->target == 0 || (go->target != 0 && !go->target->active)) {
        if (go->pos.x > m_worldWidth + go->scale.x || go->pos.x < -go->scale.x || go->pos.y > m_worldHeight + go->scale.x || go->pos.y < -go->scale.x) {
            go->SetInactive();
        }
    }
}

void SceneAss1::UpdateDrop(GameObject* go, const double dt)
{
    // MOVE THE POWERUP DROP
    go->vel += m_gravity * dt;
    go->pos += go->vel * dt;

    // Handle collision between GO_SHIP and GO_DROP using simple distance-based check
    if (m_ship->active && CheckCollision(go, m_ship)) {    //there is collision

        // Grant the drop's effect to the player
        switch (go->droptype) {
        case GameObject::DROP_ADDLIFE:
            if (m_lives < 5) {
                ++m_lives;
            }
            break;
        case GameObject::DROP_ADDBULLET:
            if (m_bullets < 5) {
                ++m_bullets;
            }
            break;
        case GameObject::DROP_BLACKHOLE:
            ++blackhole_count;
            break;
        }
        go->SetInactive();      //remove the powerup drop
        Application::GetInstance().PlayPickupSE();
    }

    // Unspawn when they leave screen
    if (go->active) {
        if (go->pos.x < -go->scale.x || go->pos.y < -go->scale.y) {
            go->SetInactive();
        }
    }

}

void SceneAss1::Update(const double dt)
{
    SceneBase::Update(dt);

    if (state == GAMEPLAY_PLAY && Application::IsKeyPressed(VK_ESCAPE))
    {
        state = GAMEPLAY_PAUSE;
    }

    switch (state)
    {
    case GAMEPLAY_PAUSE:
        PauseUpdate(dt);
        break;
    case GAMEPLAY_PLAY:
        GameUpdate(dt);
        break;
    case GAMEPLAY_WINLOSE:
        LevelTransition(dt);
        break;
    }
}

void SceneAss1::PauseUpdate(double dt)
{
    //check which button is being selected
    double x, y;
    Application::GetCursorPos(&x, &y);
    for (int i = 0; i < 2; ++i) {
        if (x > 0.35f * Application::GetWindowWidth() && x < 0.65f * Application::GetWindowWidth()
            && y >(i * 0.15f + 0.25f) * Application::GetWindowHeight() && y < (i * 0.15f + 0.32f) * Application::GetWindowHeight()) {
            button_highlighted[i] = true;
        }
        else {
            button_highlighted[i] = false;
        }
    }

    //Mouse Section
    if (!bLButtonState && Application::IsMousePressed(0))
    {
        bLButtonState = true;
    }
    else if (bLButtonState && !Application::IsMousePressed(0))
    {
        bLButtonState = false;
        for (int i = 0; i < 2; ++i) {
            if (button_highlighted[i]) {    //being selected
                Application::GetInstance().PlayButtonPressSE();

                if (i == 0) {   // Resume
                    state = GAMEPLAY_PLAY;
                }

                else if (i == 1) {  // Quit
                    Application::GetInstance().b_modeChange = true;
                    Application::GetInstance().mode = Application::GAMEMODE_MENU;
                }
                break;
            }
        }
    }
}

void SceneAss1::LevelTransition(double dt)
{
    //check which button is being selected
    double x, y;
    Application::GetCursorPos(&x, &y);
    for (int i = 0; i < 2; ++i) {
        if (x > 0.35f * Application::GetWindowWidth() && x < 0.65f * Application::GetWindowWidth()
            && y >(i * 0.15f + 0.25f) * Application::GetWindowHeight() && y < (i * 0.15f + 0.32f) * Application::GetWindowHeight()) {
            button_highlighted[i] = true;
        }
        else {
            button_highlighted[i] = false;
        }
    }

    //Mouse Section
    if (!bLButtonState && Application::IsMousePressed(0))
    {
        bLButtonState = true;
    }
    else if (bLButtonState && !Application::IsMousePressed(0))
    {
        bLButtonState = false;
        for (int i = 0; i < 2; ++i) {
            if (button_highlighted[i]) {    //being selected
                Application::GetInstance().PlayButtonPressSE();

                if (i == 0) {   // Resume

                    if (levelCount == 3 && m_lives != 0) {  //player beat the game
                        Application::GetInstance().b_modeChange = true;
                        Application::GetInstance().mode = Application::GAMEMODE_MENU;
                    }
                    else {
                        state = GAMEPLAY_PLAY;

                        if (m_lives != 0) {     //player did not die; go to next level
                            ++levelCount;
                        }
                        else {
                            //reset lives
                            m_lives = 3;
                        }

                        b_endOfWave = false;
                        if (levelCount > 1) {
                            b_boss = true;
                        }
                        else {
                            b_boss = false;
                        }
                        std::stringstream ss;
                        ss << "Level/level" << levelCount << ".txt";
                        std::string s = ss.str();
                        const char * file_path = s.c_str();
                        LoadLevel(file_path, m_levelData);
                        m_itLevelData = m_levelData.begin();
                        ResetPlayer();
                        m_ship->active = true;
                        m_enemyShip->active = false;
                        m_blackhole->active = false;
                        for (std::vector<GameObject*>::iterator it = m_goList.begin(); it != m_goList.end(); ++it) {
                            GameObject *go = (GameObject *)*it;
                            go->active = false;
                            m_itFetchGO = m_goList.begin();
                        }
                        GameObject::GO_COUNT = 0;
                    }
                }

                else if (i == 1) {  // Quit
                    if (levelCount == 3 && m_lives != 0) {  //player beat the game
                        Application::GetInstance().b_modeChange = true;
                        Application::GetInstance().mode = Application::GAMEMODE_EXIT;
                    }
                    else {
                        Application::GetInstance().b_modeChange = true;
                        Application::GetInstance().mode = Application::GAMEMODE_MENU;
                    }
                }
                break;
            }
        }
    }
}

void SceneAss1::GameUpdate(double dt)
{
    backgroundOffset += 10 * dt;
    if (backgroundOffset >= m_worldWidth) {
        backgroundOffset -= m_worldWidth;
    }

    //Mouse Section
    if (!bLButtonState && Application::IsMousePressed(0))
    {
        bLButtonState = true;
    }
    else if (bLButtonState && !Application::IsMousePressed(0))
    {
        bLButtonState = false;

        if (b_blackhole_activated) {
            b_blackhole_activated = false;
            if (m_blackhole->active) {      //there already is an active black hole
                if (SE_blackhole) {
                    SE_blackhole->stop();
                }
                ExplodeBlackhole();
                blackhole_timeElapsed = 0.0;
            }
            --blackhole_count;
            SpawnBlackHole();
            SE_blackhole = Application::GetInstance().m_soundEngine->play2D("Sound//SE_black hole.wav", true, false, true);
        }
    }
    static bool bRButtonState = false;
    if (!bRButtonState && Application::IsMousePressed(1))
    {
        bRButtonState = true;
    }
    else if (bRButtonState && !Application::IsMousePressed(1))
    {
        bRButtonState = false;

        // Disable black hole skill
        if (b_blackhole_activated) {
            b_blackhole_activated = false;
        }
    }

    // Key movement for player ship
    m_ship->force.SetZero();

    if (m_ship->active) {
        if (Application::IsKeyPressed('W'))
        {
            m_ship->force = 5.f * m_ship->direction; //direction ship is facing based on angle
        }
        if (Application::IsKeyPressed('S'))
        {
            //if (m_ship->vel.LengthSquared() >= Math::EPSILON) {
            //    m_force = -5.f * m_ship->vel.Normalized();
            //}

            m_ship->force = -5.f * m_ship->direction; //direction ship is facing based on angle
        }
        if (Application::IsKeyPressed('A'))
        {
            //m_ship->rotSpeed += 15.f / m_ship->mass;
            Mtx44 rotation;
            rotation.SetToRotation(80.f * dt, 0, 0, 1);
            m_ship->direction = rotation * m_ship->direction;
        }
        if (Application::IsKeyPressed('D'))
        {
            //m_ship->rotSpeed -= 15.f / m_ship->mass;
            Mtx44 rotation;
            rotation.SetToRotation(-80.f * dt, 0, 0, 1);
            m_ship->direction = rotation * m_ship->direction;
        }

        // Press spacebar to spawn a bullet
        if (Application::IsKeyPressed(VK_SPACE)) {
            if (bulletTimeElapsed > bulletSpawnRate) {
                Application::GetInstance().PlayBulletFireSE();
                SpawnBullet();
                bulletTimeElapsed = 0.0;
            }
        }

        // Press 'E' to activate black hole
        if (Application::IsKeyPressed('E')) {
            if (!b_blackhole_activated && blackhole_count > 0) {
                b_blackhole_activated = true;
                Application::GetInstance().PlayBulletFireSE();
            }
        }

        // Press 'R' to launch missile
        if (Application::IsKeyPressed('R')) {
            if (missile_count > 0) {
                if (missileTimeElapsed > missileSpawnRate) {
                    --missile_count;
                    SpawnMissile();
                    Application::GetInstance().PlayMissileFireSE();
                    missileTimeElapsed = 0.0;
                }
            }
        }

        // Limit the spawn rate of bullets, missiles and drops
        bulletTimeElapsed += dt;
        missileTimeElapsed += dt;
        dropTimeElapsed += dt;

        UpdatePlayerShip(dt);
    }
    else {      // !m_ship->active
        deathTimeElapsed += dt;
        if (deathTimeElapsed > 3.0) {
            m_ship->active = true;
            deathTimeElapsed = 0.0;
        }
    }

    // Update blackhole if it is activated
    if (m_blackhole->active) {
        UpdateBlackhole(dt);
    }

    if (m_enemyShip->active) {
        UpdateEnemyShip(dt);
    }

    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->active)
        {
            if (go->type == GameObject::GO_ASTEROID) {
                UpdateAsteroid(go, dt);
            }

            else if (go->type == GameObject::GO_METEOR) {
                UpdateMeteor(go, dt);
            }

            else if (go->type == GameObject::GO_BULLET) {
                UpdateBullet(go, dt);
            }

            else if (go->type == GameObject::GO_ENEMY_BULLET) {
                UpdateEnemyBullet(go, dt);
            }

            else if (go->type == GameObject::GO_MISSILE) {
                UpdateMissile(go, dt);
            }

            else if (go->type == GameObject::GO_DROP) {
                UpdateDrop(go, dt);
            }
        }
    }

    // if a large asteroid was destroyed
    if (b_spawnSmallAsteroids) {
        SpawnAsteroid(asteroidPos.x, asteroidPos.y, 1);
        SpawnAsteroid(asteroidPos.x, asteroidPos.y, 1);
        SpawnAsteroid(asteroidPos.x, asteroidPos.y, 1);
        b_spawnSmallAsteroids = false;
    }

    // Spawn for the level
    if (!b_endOfWave) {
        SpawnWave(dt);
    }
    else if (b_boss && GameObject::GO_COUNT <= 0 && !m_enemyShip->active) {  //there is a boss fight
        SpawnEnemyShip(static_cast<GameObject::ENEMY_TYPE>(GameObject::ENEMY_BOSS1 + levelCount - 2));
    }
    else if (b_endOfWave && !b_boss && GameObject::GO_COUNT <= 0 && !m_blackhole->active) {
        //level complete
        state = GAMEPLAY_WINLOSE;
    }

}

void SceneAss1::SpawnAsteroid(float posX, float posY, int hp)
{
    GameObject *go = FetchGO();
    go->type = GameObject::GO_ASTEROID;
    go->scale.Set(2.5f * hp, 2.5f * hp, 2.5f * hp);
    go->hp = hp;
    go->angle = Math::RandFloatMinMax(0.f, 360.f);
    go->rotSpeed = Math::RandFloatMinMax(10.f, 40.f);

    go->pos.Set(posX, posY, 0);

    go->vel.Set(Math::RandFloatMinMax(-20.f, -5.f), Math::RandFloatMinMax(-2.5f, 0.f), 0);
}

void SceneAss1::SpawnMeteor(float posX, float posY)
{
    GameObject *go = FetchGO();
    go->type = GameObject::GO_METEOR;
    go->scale.Set(12.f, 12.f, 12.f);

    go->pos.Set(posX, posY, 0);

    go->vel.Set(Math::RandFloatMinMax(-15.f, -5.f), 0.f, 0);
}

void SceneAss1::SpawnEnemyShip(GameObject::ENEMY_TYPE type)
{
    m_enemyShip->active = true;
    m_enemyShip->bosstype = type;

    if (m_enemyShip->bosstype == GameObject::ENEMY_BOSS1)
    {
        m_enemyShip->scale.Set(4.f, 4.f, 4.f);
        m_enemyShip->hp = 8;     //take 8 hits to kill
        m_enemyShip->mass = 15;
        m_enemyShip->force.SetZero();
        m_enemyShip->direction.Set(-1, 0, 0);
        m_enemyShip->pos.Set(m_worldWidth + 5.f, m_worldHeight * 0.5f, 0);
        m_enemyShip->vel.Set(-1, 0, 0);

        m_enemyShip->shootTimeElapsed = 0.0;
        m_enemyShip->shootRate = 0.5;
    }
    
    else if (m_enemyShip->bosstype == GameObject::ENEMY_BOSS2)
    {
        m_enemyShip->scale.Set(8.f, 8.f, 8.f);
        m_enemyShip->hp = 12;     //take 12 hits to kill
        m_enemyShip->mass = 15;
        m_enemyShip->force.SetZero();
        m_enemyShip->direction.Set(-1, 0, 0);
        m_enemyShip->pos.Set(m_worldWidth + 2.f * m_enemyShip->scale.x, m_worldHeight * 0.5f, 0);
        m_enemyShip->vel.Set(-5.f, 0, 0);
        m_enemyShip->angle = 0.f;
        m_enemyShip->rotSpeed = -1.f;

        m_enemyShip->shootTimeElapsed = 0.0;
        m_enemyShip->shootRate = 0.5;
    }
}

void SceneAss1::SpawnEnemyBullet()
{
    //if (m_enemyShip->bosstype == GameObject::ENEMY_BOSS1) {
    //    
    //    // use angle to store the previous bullet angle
    //    m_enemyShip->angle += m_enemyShip->rotSpeed * 30.f;
    //    if (m_enemyShip->angle > 120.f || m_enemyShip->angle < -120.f) {
    //        m_enemyShip->rotSpeed *= -1;
    //    }
    //
    //    float shootAngle = Math::RadianToDegree(atan2(m_ship->direction.y, m_ship->direction.x)) + m_enemyShip->angle;
    //
    //    GameObject *go = FetchGO();
    //    go->type = GameObject::GO_ENEMY_BULLET;
    //    go->scale.Set(1.f, 1.f, 1.f);
    //    go->mass = 0.2;
    //    Vector3 bulletDirection = Vector3(cos(Math::DegreeToRadian(shootAngle)), sin(Math::DegreeToRadian(shootAngle)), 0);
    //    go->pos = m_enemyShip->pos + m_enemyShip->scale.x * bulletDirection;
    //    go->vel = m_enemyShip->direction * E_BULLET_SPEED;
    //}

    GameObject *go = FetchGO();
    go->type = GameObject::GO_ENEMY_BULLET;
    go->scale.Set(1.f, 1.f, 1.f);
    go->mass = 0.2;
    go->pos = m_enemyShip->pos + m_enemyShip->scale.x * m_enemyShip->direction;
    go->vel = m_enemyShip->direction * E_BULLET_SPEED;
}

void SceneAss1::SpawnBullet()
{
    float startingAngle = Math::RadianToDegree(atan2(m_ship->direction.y, m_ship->direction.x)) - (m_bullets - 1) * 10.f;
    Vector3 direction(cos(Math::DegreeToRadian(startingAngle)), sin(Math::DegreeToRadian(startingAngle)), 0);

    for (int i = 0; i < m_bullets; ++i) {
        GameObject *go = FetchGO();
        go->type = GameObject::GO_BULLET;
        go->scale.Set(1.5f, 1.5f, 1.5f);
        go->mass = 0.2;
        go->pos = m_ship->pos + m_ship->scale.x * direction;
        go->vel = direction * BULLET_SPEED;

        if (i != m_bullets - 1) {   //spawned last bullet already; don't need to rotate direction
            Mtx44 rotation;
            rotation.SetToRotation(20.f, 0, 0, 1);
            direction = rotation * direction;
        }
    }

}

void SceneAss1::SpawnMissile()
{
    float startingAngle = Math::RadianToDegree(atan2(m_ship->direction.y, m_ship->direction.x));
    int randomiser = Math::RandIntMinMax(0, m_goList.size());
    int count = 0;  //to check if it is the randomised selected game object

    GameObject* getTarget = 0;

    if (m_enemyShip->active) {
        getTarget = m_enemyShip;
    }
    else
    {
        for (std::vector<GameObject*>::iterator it = m_goList.begin(); count <= randomiser && it != m_goList.end(); (it == m_goList.end() - 1) ? (it = m_goList.begin()) : (++it))
        {
            GameObject *go = (GameObject *)*it;
            if (!go->active) {
                if (count == 0 && it == m_goList.end() - 1) {     //end of list
                    break;
                }
                continue;
            }
            if (go->type == GameObject::GO_MISSILE) {
                continue;
            }
            if (go->type == GameObject::GO_BULLET) {
                continue;
            }
            if (go->type == GameObject::GO_DROP) {
                continue;
            }

            if (count == randomiser) {
                getTarget = go;
                break;
            }
            ++count;
        }
    }

    GameObject *go = FetchGO();
    go->type = GameObject::GO_MISSILE;
    go->scale.Set(1.5f, 1.5f, 1.5f);
    go->mass = 1;
    go->force.SetZero();
    go->direction = m_ship->direction;
    go->pos = m_ship->pos + Vector3(m_ship->scale.x, m_ship->scale.y, 1) + go->direction;
    go->vel = go->direction * MISSILE_SPEED;
    go->target = getTarget;
}

void SceneAss1::SpawnBlackHole()
{
    m_blackhole->active = true;
    m_blackhole->angle = 0.f;

    double x, y;
    Application::GetCursorPos(&x, &y);
    int w = Application::GetWindowWidth();
    int h = Application::GetWindowHeight();

    float worldX = x / w * m_worldWidth;
    float worldY = (h - y) / h * m_worldHeight;

    m_blackhole->pos.Set(worldX, worldY, 0);
    
}

bool SceneAss1::CalculateDropRate(const double dt)
{
    if (dropTimeElapsed > dropSpawnRate) {
        float rate = Math::RandFloatMinMax(0.f, 1.f);   //check chance of something dropping
        if (rate < 0.15f) {  //10% chance of drops
            dropTimeElapsed = 0.0;
            return true;
        }
    }
    return false;
}

void SceneAss1::SpawnDrop(float posX, float posY)
{
    GameObject *go = FetchGO();
    go->type = GameObject::GO_DROP;
    float rate = Math::RandFloatMinMax(0.f, 1.f);   //check what will drop
    if (rate < 0.2f) {      //20% chance it is life
        go->droptype = GameObject::DROP_ADDLIFE;
    }
    else if (rate < 0.4f) {     //20% chance it is black hole
        go->droptype = GameObject::DROP_BLACKHOLE;
    }
    else {  //60% chance it is bullet
        go->droptype = GameObject::DROP_ADDBULLET;
    }

    go->scale.Set(3.f, 3.f, 3.f);
    go->pos.Set(posX, posY, 0);
    go->vel.Set(-10.f, 0, 0);   //to move it across the screen
}

void SceneAss1::PlayerGetHit()
{
    if (m_ship->active) {
        m_ship->active = false;     //temporary death
        ResetPlayer();
        Application::GetInstance().PlayExplosionSE();

        --m_lives;  //deduct 1 life
        m_bullets = 1;  //reset bullets to 1

        if (m_lives == 0) {     //game lost; reset the game
            state = GAMEPLAY_WINLOSE;
        }

    }
}

void SceneAss1::ResetPlayer()
{
    m_ship->pos.Set(m_worldWidth * 0.1f, m_worldHeight * 0.5f, 0);  //respawn ship at original position
    m_ship->direction.Set(1, 0, 0);
    m_ship->vel.SetZero();  //reset velocity
}

void SceneAss1::SpawnWave(const double dt)
{
    waveTimeElapsed += dt;
    if (waveTimeElapsed >= (*m_itLevelData)->waitTime) {
        
        int worldX, worldY;     //randomised coordinates for spawning

        std::string spawnData = (*m_itLevelData)->spawn;
        for (unsigned i = 0; i < spawnData.size(); ++i) {

            worldX = Math::RandFloatMinMax(m_worldWidth - 80.f, m_worldWidth + 10.f);
            worldY = Math::RandFloatMinMax(m_worldHeight, m_worldHeight + 10.f);

            if (spawnData[i] == 'a') {
                SpawnAsteroid(worldX, worldY, 1);
            }
            else if (spawnData[i] == 'A') {
                SpawnAsteroid(worldX, worldY, 2);
            }
            else if (spawnData[i] == 'm') {
                SpawnMeteor(worldX, worldY);
            }
        }

        waveTimeElapsed = 0.0;  //reset timer
        if (m_itLevelData != (m_levelData.end() - 1)) {
            ++m_itLevelData;    //move iterator forward
        }
        else {  //end of wave already
            //end the level/spawn the boss
            b_endOfWave = true;
        }
    }
}

bool SceneAss1::CheckCollision(GameObject *go, GameObject *go2)
{
    float distSquare = (go->pos - go2->pos).LengthSquared();
    float combinedRadiusSquare = (go->scale.x + go2->scale.x) * (go->scale.y + go2->scale.y);

    if (distSquare <= combinedRadiusSquare) {
        return true;
    }
    return false;
}

void SceneAss1::RenderGO(GameObject *go)
{
    switch (go->type)
    {
    case GameObject::GO_SHIP:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        //modelStack.Rotate(go->angle, 0, 0, 1);
        modelStack.Rotate(Math::RadianToDegree(atan2(go->direction.y, go->direction.x)), 0, 0, 1);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_SHIP], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_ASTEROID:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Rotate(go->angle, 0, 0, 1);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_ASTEROID], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_METEOR:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_METEOR], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_BULLET:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_BULLET], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_MISSILE:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_MISSILE], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_ENEMY_SHIP:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Rotate(Math::RadianToDegree(atan2(go->direction.y, go->direction.x)), 0, 0, 1);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        if (go->bosstype == GameObject::ENEMY_BOSS1) {
            RenderMesh(meshList[GEO_ENEMY_SHIP_1], false);
        }
        else if (go->bosstype == GameObject::ENEMY_BOSS2) {
            RenderMesh(meshList[GEO_ENEMY_SHIP_2], false);
        }
        modelStack.PopMatrix();
        break;

    case GameObject::GO_ENEMY_BULLET:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Rotate(Math::RadianToDegree(atan2(go->vel.y, go->vel.x)), 0, 0, 1);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        RenderMesh(meshList[GEO_ENEMY_BULLET], false);
        modelStack.PopMatrix();
        break;

    case GameObject::GO_DROP:
        modelStack.PushMatrix();
        modelStack.Translate(0, 0, m_renderCount);
        modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
        modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
        if (go->droptype == GameObject::DROP_ADDLIFE) {
            RenderMesh(meshList[GEO_DROP_ADDLIFE], false);
        }
        else if (go->droptype == GameObject::DROP_ADDBULLET) {
            RenderMesh(meshList[GEO_DROP_ADDBULLET], false);
        }
        else if (go->droptype == GameObject::DROP_BLACKHOLE) {
            RenderMesh(meshList[GEO_DROP_BLACKHOLE], false);
        }
        modelStack.PopMatrix();
        break;

    case GameObject::GO_BLACKHOLE:
        RenderBlackHole(go->pos.x, go->pos.y, go->angle, false);
        break;
    }
}

void SceneAss1::RenderBackground()
{
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 1.5f - backgroundOffset, m_worldHeight * 0.5f, -1);

    modelStack.PushMatrix();
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_BACKGROUND], false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(-m_worldWidth, 0, 0);
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_BACKGROUND], false);
    modelStack.PopMatrix();

    modelStack.PopMatrix();
}

void SceneAss1::RenderInfoOnScreen()
{
    // Lives
    for (int i = 0; i < m_lives; ++i) {
        modelStack.PushMatrix();
        modelStack.Translate(4.f + i * 8.f, m_worldHeight - 4.f, 7.f);
        modelStack.Scale(4.f, 4.f, 4.f);
        RenderMesh(meshList[GEO_HEART], false);
        modelStack.PopMatrix();
    }
    
    // black hole ammo
    for (int i = 0; i < blackhole_count; ++i) {
        modelStack.PushMatrix();
        modelStack.Translate(3.f + i * 6.f, m_worldHeight - 11.f, 7.f);
        modelStack.Scale(3.f, 3.f, 3.f);
        RenderMesh(meshList[GEO_BLACKHOLE], false);
        modelStack.PopMatrix();
    }

    // missile ammo
    for (int i = 0; i < missile_count; ++i) {
        modelStack.PushMatrix();
        modelStack.Translate(2.f + i * 4.f, m_worldHeight - 17.f, 7.f);
        modelStack.Scale(2.f, 2.f, 2.f);
        RenderMesh(meshList[GEO_MISSILE], false);
        modelStack.PopMatrix();
    }

    //missile target locks
    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->type == GameObject::GO_MISSILE && go->active) {
            modelStack.PushMatrix();
            modelStack.Translate(0, 0, 9.f);
            if (go->target != 0 && go->target->active) {
                modelStack.Translate(go->target->pos.x, go->target->pos.y, go->target->pos.z);
                modelStack.Scale(go->target->scale.x, go->target->scale.y, go->target->scale.z);
            }
            RenderMesh(meshList[GEO_TARGETLOCK], false);
            modelStack.PopMatrix();
        }
    }

    // Score
    std::stringstream ss;
    ss << "Score: " << m_score;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 40, 55);

    // GameObject count
    ss.str("");
    ss << "GO COUNT:" << GameObject::GO_COUNT;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 0), 2, 40, 52);

    // FPS
    ss.str("");
    ss.precision(5);
    ss << "FPS: " << fps;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 1), 2, 0, 3);

    // Level
    ss.str("");
    ss << "Level " << levelCount;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 1), 3, 0, 0);
}

void SceneAss1::RenderBlackHole(float posX, float posY, float angle, bool ghost)
{
    modelStack.PushMatrix();
    if (ghost) {
        modelStack.Translate(posX, posY, -0.85f);
    }
    else {
        modelStack.Translate(posX, posY, -0.9f);
    }

    modelStack.PushMatrix();
    modelStack.Rotate(90, 1, 0, 0);
    modelStack.Scale(2.f * m_blackhole->scale.x, 2.f * m_blackhole->scale.y, 2.f * m_blackhole->scale.z);
    RenderMesh(meshList[GEO_BLACKHOLE_RANGE], false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Rotate(angle, 0, 0, 1);
    modelStack.Scale(m_blackhole->scale.x, m_blackhole->scale.y, m_blackhole->scale.z);
    RenderMesh(meshList[GEO_BLACKHOLE], false);
    modelStack.PopMatrix();

    modelStack.PopMatrix();
}

void SceneAss1::RenderPause()
{
    // Pause box
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.6f, 9.f);
    modelStack.Scale(m_worldWidth * 0.5f, m_worldHeight * 0.7f, 1.f);
    RenderMesh(meshList[GEO_PAUSEBOX], false);
    modelStack.PopMatrix();

    // Resume button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.7f, 9.5f);
    if (button_highlighted[0]) {
        if (bLButtonState) {
            modelStack.Scale(m_worldWidth * 0.25f, m_worldHeight * 0.07f, 1.f);
        }
        else {
            modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        }
        RenderMesh(meshList[GEO_BUTTON_HIGHLIGHTED], false);
    }
    else {
        modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    }
    RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    modelStack.PopMatrix();

    // Quit button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.55f, 9.5f);
    if (button_highlighted[1]) {
        if (bLButtonState) {
            modelStack.Scale(m_worldWidth * 0.25f, m_worldHeight * 0.07f, 1.f);
        }
        else {
            modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        }
        RenderMesh(meshList[GEO_BUTTON_HIGHLIGHTED], false);
    }
    else {
        modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    }
    RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    modelStack.PopMatrix();

    RenderTextOnScreen(meshList[GEO_MENUTEXT], "PAUSE", Color(1, 0, 1), 3, 34, 50);

    RenderTextOnScreen(meshList[GEO_MENUTEXT], "RESUME", Color(1, 0, 1), 3, 32, 40.5f);
    RenderTextOnScreen(meshList[GEO_MENUTEXT], "QUIT", Color(1, 0, 1), 3, 35, 31.5f);
}

void SceneAss1::RenderLevelTransition()
{
    // Splash screen
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 9.f);
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_PAUSEBOX], false);
    modelStack.PopMatrix();

    // Continue button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.7f, 9.5f);
    if (button_highlighted[0]) {
        if (bLButtonState) {
            modelStack.Scale(m_worldWidth * 0.25f, m_worldHeight * 0.07f, 1.f);
        }
        else {
            modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        }
        RenderMesh(meshList[GEO_BUTTON_HIGHLIGHTED], false);
    }
    else {
        modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    }
    RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    modelStack.PopMatrix();

    // Quit button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.55f, 9.5f);
    if (button_highlighted[1]) {
        if (bLButtonState) {
            modelStack.Scale(m_worldWidth * 0.25f, m_worldHeight * 0.07f, 1.f);
        }
        else {
            modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        }
        RenderMesh(meshList[GEO_BUTTON_HIGHLIGHTED], false);
    }
    else {
        modelStack.Scale(m_worldWidth * 0.3f, m_worldHeight * 0.075f, 1.f);
        RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    }
    RenderMesh(meshList[GEO_BUTTON_NORMAL], false);
    modelStack.PopMatrix();

    if (m_lives == 0) {     //player lost
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "YOU DIED", Color(1, 0, 1), 3, 31, 50);
    }
    else {      //player beat the level
        if (levelCount == 3) {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "YOU WIN!", Color(1, 0, 1), 3, 31, 50);
        }
        else {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "LEVEL COMPLETE", Color(1, 0, 1), 3, 22, 50);
        }
    }

    if (m_lives == 0 || levelCount < 3) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "CONTINUE", Color(1, 0, 1), 3, 29, 40.5f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "MAIN MENU", Color(1, 0, 1), 2, 31.5f, 41.f);
    }
    RenderTextOnScreen(meshList[GEO_MENUTEXT], "QUIT", Color(1, 0, 1), 3, 35, 31.5f);

    // Level
    std::stringstream ss;
    ss << "Level " << levelCount;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 1), 3, 0, 0);

    // Score
    ss.str("");
    ss << "Score: " << m_score;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 40, 55);
}

void SceneAss1::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Calculating aspect ratio
    m_worldHeight = 100.f;
    m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

    // Projection matrix : Orthographic Projection
    Mtx44 projection;
    projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
    projectionStack.LoadMatrix(projection);

    // Camera matrix
    viewStack.LoadIdentity();
    viewStack.LookAt(
        camera.position.x, camera.position.y, camera.position.z,
        camera.target.x, camera.target.y, camera.target.z,
        camera.up.x, camera.up.y, camera.up.z
        );
    // Model matrix : an identity matrix (model will be at the origin)
    modelStack.LoadIdentity();

    // Render game background
    RenderBackground();

    if (m_blackhole->active) {
        RenderGO(m_blackhole);
    }

    // Render black hole if skill is activated
    if (b_blackhole_activated) {
        double x, y;
        Application::GetCursorPos(&x, &y);
        int w = Application::GetWindowWidth();
        int h = Application::GetWindowHeight();

        float worldX = x / w * m_worldWidth;
        float worldY = (h - y) / h * m_worldHeight;
        RenderBlackHole(worldX, worldY);
    }

    m_renderCount = 0.f;
    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->active)
        {
            RenderGO(go);
            m_renderCount += 0.1f;
        }
    }

    if (m_enemyShip->active) {
        RenderGO(m_enemyShip);
    }
    if (m_ship->active) {
        RenderGO(m_ship);
    }

    //On screen information
    RenderInfoOnScreen();

    if (state == GAMEPLAY_PAUSE) {
        RenderPause();
    }

    if (state == GAMEPLAY_WINLOSE) {
        RenderLevelTransition();
    }
}

void SceneAss1::Exit()
{
    SceneBase::Exit();
    //Cleanup GameObjects
    while (m_goList.size() > 0)
    {
        GameObject *go = m_goList.back();
        delete go;
        m_goList.pop_back();
    }
    
    ClearWaveData(m_levelData);

    if (m_ship)
    {
        delete m_ship;
        m_ship = NULL;
    }

    if (m_blackhole)
    {
        delete m_blackhole;
        m_blackhole = NULL;
    }

    if (SE_blackhole) {
        SE_blackhole->drop();
    }

    if (m_enemyShip)
    {
        delete m_enemyShip;
        m_enemyShip = NULL;
    }
}