#include "SceneGame.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneGame::SceneGame() : MAX_SPEED(1), BULLET_SPEED(100), MISSILE_SPEED(20), E_BULLET_SPEED(50)
{
}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
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

    b_spawnSmallAsteroids = false;

    // Pause screen
    button_highlighted[0] = false;
    button_highlighted[1] = false;
    bLButtonState = false;
}

GameObject* SceneGame::FetchGO()
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

void SceneGame::Update(const double dt)
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
        //LevelTransition(dt);
        break;
    }
}

void SceneGame::PauseUpdate(double dt)
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
                    //Application::GetInstance().b_modeChange = true;
                    //Application::GetInstance().mode = Application::GAMEMODE_MENU;
                }
                break;
            }
        }
    }
}

void SceneGame::GameUpdate(double dt)
{

}

void SceneGame::PlayerGetHit()
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

void SceneGame::ResetPlayer()
{
    m_ship->pos.Set(m_worldWidth * 0.1f, m_worldHeight * 0.5f, 0);  //respawn ship at original position
    m_ship->direction.Set(1, 0, 0);
    m_ship->vel.SetZero();  //reset velocity
}

bool SceneGame::CheckCollision(GameObject *go, GameObject *go2)
{
    float distSquare = (go->pos - go2->pos).LengthSquared();
    float combinedRadiusSquare = (go->scale.x + go2->scale.x) * (go->scale.y + go2->scale.y);

    if (distSquare <= combinedRadiusSquare) {
        return true;
    }
    return false;
}

void SceneGame::RenderGO(GameObject *go)
{
    switch (go->type)
    {
    }
}

void SceneGame::RenderBackground()
{
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_BACKGROUND], false);
    modelStack.PopMatrix();
}

void SceneGame::RenderPlayer()
{
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 1.f);
    modelStack.Scale(10.f, 10.f, 10.f);
    RenderMesh(meshList[GEO_PLAYER], false);
    modelStack.PopMatrix();
}

void SceneGame::RenderInfoOnScreen()
{
    // Score
    std::stringstream ss;
    ss << "Score: " << m_score;
    RenderTextOnScreen(meshList[GEO_CALIBRITEXT], ss.str(), Color(1, 1, 1), 3, 40, 55);
}

void SceneGame::RenderPause()
{
    //// Pause box
    //modelStack.PushMatrix();
    //modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.6f, 9.f);
    //modelStack.Scale(m_worldWidth * 0.5f, m_worldHeight * 0.7f, 1.f);
    //RenderMesh(meshList[GEO_PAUSEBOX], false);
    //modelStack.PopMatrix();

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

void SceneGame::RenderLevelTransition()
{
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
    RenderTextOnScreen(meshList[GEO_CALIBRITEXT], ss.str(), Color(0, 1, 1), 3, 0, 0);

    // Score
    ss.str("");
    ss << "Score: " << m_score;
    RenderTextOnScreen(meshList[GEO_CALIBRITEXT], ss.str(), Color(1, 1, 1), 3, 40, 55);
}

void SceneGame::Render()
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

    // Render Player
    RenderPlayer();

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

    //On screen information
    RenderInfoOnScreen();

    if (state == GAMEPLAY_PAUSE) {
        RenderPause();
    }

    if (state == GAMEPLAY_WINLOSE) {
        RenderLevelTransition();
    }
}

void SceneGame::Exit()
{
    SceneBase::Exit();
    //Cleanup GameObjects
    while (m_goList.size() > 0)
    {
        GameObject *go = m_goList.back();
        delete go;
        m_goList.pop_back();
    }

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
}