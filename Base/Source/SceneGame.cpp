#include "SceneGame.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneGame::SceneGame()
{
}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
{
    SceneBase::Init();

	m_cmap = new CMap();
	m_cmap->Init(Application::GetWindowHeight(), Application::GetWindowWidth(), 32);
	m_cmap->LoadFile("Level//testMap.csv");
    Math::InitRNG();

    // World Coordinates
    m_worldHeight = 100.f;
    m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

    // Construct 10 GameObject with type GO_ASTEROID and add into m_goList
    for (unsigned i = 0; i < 10; ++i) {
        m_goList.push_back(new Projectile());
    }
    m_itFetchGO = m_goList.begin();
    m_renderCount = 0.f;

    // Init Player
    m_player1 = new Player();
    m_player1->Init();
	m_player1->controllerID = 0;
	m_player1->SetScale(Vector3(32, 32, 1));
	m_player2 = new Player();
	m_player2->Init();
	m_player2->controllerID = 1;
	m_player2->SetScale(Vector3(32, 32, 1));

    // Enemy
    Enemy* enemy = new Enemy();
    enemy->SetActive(true);
    enemy->SetPos(Vector3(10, 10, 1));
    enemy->SetScale(Vector3(5, 5, 5));   
    enemy->SetTarget(m_player1->GetPos());
    enemy->SetEnemyType(Enemy::RANGED);
    enemy->SetMesh(meshList[GEO_PLAYER]);
    enemy->SetType(GameObject::ENEMY);
    m_goList.push_back(enemy);



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
        if (!go->GetActive()) {      //object is inactive
            go->SetActive(true);
            m_itFetchGO = it;
            return go;
        }

    }

    GameObject *go = (GameObject *)*(m_goList.end() - 1);
    go->SetActive(true);
    m_itFetchGO = m_goList.begin();
    return go;
}

void SceneGame::Update(const double dt)
{
    SceneBase::Update(dt);
	m_player1->Update(dt, m_cmap);
	m_player2->Update(dt, m_cmap);

    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->GetActive())
        {
            if (go->GetType() == GameObject::ENEMY)
            {
                dynamic_cast<Enemy*>(go)->Update(dt, m_player1->GetPos());
            }
        }
    }

    // Loop thru objects
    for (std::vector<GameObject*>::size_type i = 0; i < m_goList.size(); ++i)
    {
        GameObject* go = m_goList[i];
        for (std::vector<GameObject*>::size_type i2 = 0; i2 < m_goList.size(); ++i2)
        {
            GameObject *go2 = m_goList[i2];
            if (go->CheckCollisionWith(go2))
            {
                Interactions(go, go2);
            }
        }
    }

    // Loop for player projectiles
    for (std::vector<Projectile*>::size_type i = 0; i < m_player1->m_ProjectileList.size(); ++i)
    {
        Projectile* go = m_player1->m_ProjectileList[i];
        for (std::vector<GameObject*>::size_type i2 = 0; i2 < m_goList.size(); ++i2)
        {
            GameObject *go2 = m_goList[i2];
            if (go->CheckCollisionWith(go2))
            {
                Interactions(go, go2);
            }
        }
    }

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

void SceneGame::Interactions(GameObject* go1, GameObject* go2)
{
    switch (go1->GetType())
    {
    case GameObject::PROJECTILE_MELEE:
    {
        if (go2->GetType() == GameObject::SWITCH)
            dynamic_cast<Switch*>(go2)->InteractionResponse(go1);

        if (go2->GetType() == GameObject::ENEMY)
        {
            dynamic_cast<Enemy*>(go2)->TakeDamage(dynamic_cast<Projectile*>(go1)->GetDmg());
            go1->SetActive(false);
        }
        break;
    }

    case GameObject::PROJECTILE_RANGED:
    {
        if (go2->GetType() == GameObject::SWITCH)
            dynamic_cast<Switch*>(go2)->InteractionResponse(go1);
        if (go2->GetType() == GameObject::ENEMY)
        {
            dynamic_cast<Enemy*>(go2)->TakeDamage(dynamic_cast<Projectile*>(go1)->GetDmg());
            go1->SetActive(false);
        }
        break;
    }
    }
}

void SceneGame::PlayerGetHit()
{
    //if (m_ship->active) {
    //    m_ship->active = false;     //temporary death
    //    ResetPlayer();
    //    Application::GetInstance().PlayExplosionSE();

    //    --m_lives;  //deduct 1 life
    //    m_bullets = 1;  //reset bullets to 1

    //    if (m_lives == 0) {     //game lost; reset the game
    //        state = GAMEPLAY_WINLOSE;
    //    }

    //}
}

void SceneGame::ResetPlayer()
{
    //m_ship->pos.Set(m_worldWidth * 0.1f, m_worldHeight * 0.5f, 0);  //respawn ship at original position
    //m_ship->direction.Set(1, 0, 0);
    //m_ship->vel.SetZero();  //reset velocity
}

bool SceneGame::CheckCollision(GameObject *go, GameObject *go2)
{
    //float distSquare = (go->pos - go2->pos).LengthSquared();
    //float combinedRadiusSquare = (go->scale.x + go2->scale.x) * (go->scale.y + go2->scale.y);

    //if (distSquare <= combinedRadiusSquare) {
    //    return true;
    //}
    //return false;
    return false;
}

void SceneGame::RenderGO(GameObject *go)
{
    switch (go->GetType())
    {
    case GameObject::PROJECTILE_RANGED:
    {
        modelStack.PushMatrix();
        modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
        modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
        RenderMesh(meshList[GEO_PLAYER], false);
        modelStack.PopMatrix();
        break;
    }

    case GameObject::PROJECTILE_MELEE:
    {
                                          modelStack.PushMatrix();
                                          modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
                                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
                                          RenderMesh(meshList[GEO_PLAYER], false);
                                          modelStack.PopMatrix();
                                          break;
    }

    case GameObject::ENEMY:
    {
                                          modelStack.PushMatrix();
                                          modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
                                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
                                          RenderMesh(go->GetMesh(), false);
                                          modelStack.PopMatrix();
                                          break;
    }
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
    modelStack.Translate(m_player1->GetPos().x, m_player1->GetPos().y, m_player1->GetPos().z);
	modelStack.Scale(m_cmap->GetTileSize(), m_cmap->GetTileSize(), m_cmap->GetTileSize());
    RenderMesh(meshList[GEO_PLAYER], false);
    modelStack.PopMatrix();

	modelStack.PushMatrix();
    modelStack.Translate(m_player2->GetPos().x, m_player2->GetPos().y, m_player2->GetPos().z);
	modelStack.Scale(m_cmap->GetTileSize(), m_cmap->GetTileSize(), m_cmap->GetTileSize());
	RenderMesh(meshList[GEO_PLAYER], false);
	modelStack.PopMatrix();
}

void SceneGame::RenderRayTracing()
{
	Vector3 dir = Application::GetRightStickPos(0);
    float angle = Math::RadianToDegree(atan2(dir.y, dir.x));

    // Render ray
    glLineWidth(3.f);

	if (!dir.IsZero())
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_player1->GetPos().x + m_player1->GetScale().x * 0.5, m_player1->GetPos().y + m_player1->GetScale().y * 0.5, m_player1->GetPos().z);
		modelStack.Rotate(-angle, 0, 0, 1);
		modelStack.Scale(500, 1, 1);
		RenderMesh(meshList[GEO_RAY], false);
		modelStack.PopMatrix();
	}


	dir = Application::GetRightStickPos(1);
	angle = Math::RadianToDegree(atan2(dir.y, dir.x));
	if (!dir.IsZero())
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_player2->GetPos().x + m_player2->GetScale().x * 0.5, m_player2->GetPos().y + m_player2->GetScale().y * 0.5, m_player2->GetPos().z);
		modelStack.Rotate(-angle, 0, 0, 1);
		modelStack.Scale(500, 1, 1);
		RenderMesh(meshList[GEO_RAY], false);
		modelStack.PopMatrix();
	}
    glLineWidth(1.f);
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
    projection.SetToOrtho(0, 800, 0, 600, -10, 10);
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
	glDisable(GL_DEPTH_TEST);
	RenderTileMap(m_cmap, m_player1);

    // Render Player
    RenderPlayer();

    m_renderCount = 0.f;
    for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->GetActive())
        {
            RenderGO(go);
            m_renderCount += 0.1f;

            if (go->GetType() == GameObject::ENEMY)
            {
                for (std::vector<Projectile*>::iterator it = dynamic_cast<Enemy*>(go)->m_ProjectileList.begin(); it != dynamic_cast<Enemy*>(go)->m_ProjectileList.end(); ++it)
                {
                    Projectile *go = (Projectile *)*it;
                    if (go->GetActive())
                    {
                        RenderGO(go);
                        m_renderCount += 0.1f;
                    }
                }
            }
        }
    }

    // Render Projectiles
    for (std::vector<Projectile*>::iterator it = m_player1->m_ProjectileList.begin(); it != m_player1->m_ProjectileList.end(); ++it)
    {
        Projectile *go = (Projectile *)*it;
        if (go->GetActive())
        {
            RenderGO(go);
            m_renderCount += 0.1f;
        }
    }

    RenderRayTracing();
	glEnable(GL_DEPTH_TEST);
    //On screen information
    //RenderInfoOnScreen();

    //if (state == GAMEPLAY_PAUSE) {
    //    RenderPause();
    //}

    //if (state == GAMEPLAY_WINLOSE) {
    //    RenderLevelTransition();
    //}
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
    if (m_player1)
    {
        delete m_player1;
        m_player1 = NULL;
    }


    //if (m_ship)
    //{
    //    delete m_ship;
    //    m_ship = NULL;
    //}
}