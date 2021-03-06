#include "SceneGame.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>
#include <iomanip>

SceneGame::SceneGame()
{
}

SceneGame::~SceneGame()
{
}

void SceneGame::Init()
{
	Application::GetInstance().PlayGameBGM();

    SceneBase::Init();

	//m_cmap = new CMap();
	//m_cmap->Init(Application::GetWindowHeight(), Application::GetWindowWidth(), 32);
	//m_cmap->LoadFile("Level//testMap.csv");

 //   m_spawnmap = new CMap();
 //   m_spawnmap->Init(Application::GetWindowHeight(), Application::GetWindowWidth(), 32);
 //   m_spawnmap->LoadFile("Level//spawnMap.csv");


    m_currLevel = new Level();
    m_currLevel->Init(Application::GetWindowHeight(), Application::GetWindowWidth(), 32, Level::LEVEL1);
    

    Math::InitRNG();

    NEXTLEVELONCE = false;

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
	m_player1->SetPos(Vector3(32, 32, 0));
	m_player1->SetScale(Vector3(32, 32, 1));
	m_player1->SetMesh(meshList[GEO_PLAYER1]);
    m_player1->m_attackType = Player::MELEE;
	m_player2 = new Player();
	m_player2->Init();
	m_player2->controllerID = 1;
	m_player2->SetPos(Vector3(32, 32, 0));
	m_player2->SetScale(Vector3(32, 32, 1));
	m_player2->SetMesh(meshList[GEO_PLAYER2]);
    m_player2->m_attackType = Player::RANGED;

    // Enemy
    //Enemy* enemy = new Enemy();
    //enemy->SetActive(true);
    //enemy->SetPos(Vector3(64, 64, 1));
    //enemy->SetScale(Vector3(32, 32, 5));   
    //enemy->SetTarget(m_player1->GetPos());
    //enemy->SetEnemyType(Enemy::RANGED);
    //enemy->SetMesh(meshList[GEO_PLAYER1]);
    //enemy->SetType(GameObject::ENEMY);
    //m_goList.push_back(enemy);

    SpawnObjects(m_currLevel->m_SpawnMap);

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
    m_player1->Update(dt, m_currLevel->m_TerrainMap, m_currLevel->m_SpawnMap);
    m_player2->Update(dt, m_currLevel->m_TerrainMap, m_currLevel->m_SpawnMap);

	if (Application::GetLeftStickPos(0).Length() > 0.1)
	{
		SpriteAnimation *sa = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER1_WALK]);
		if (sa)
		{
			sa->m_anim->animActive = true;
			sa->Update(dt);
			m_player1->SetMesh(meshList[GEO_PLAYER1_WALK]);
		}
	}
	else
	{
		m_player1->SetMesh(meshList[GEO_PLAYER1]);
	}

	if (Application::IsButtonPressed(0, Application::R2))
	{
		SpriteAnimation *sa = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER1_ATTACK]);
		if (sa)
		{
			sa->m_anim->animActive = true;
			sa->Update(dt);
			m_player1->SetMesh(meshList[GEO_PLAYER1_ATTACK]);
		}
	}

	if (Application::GetLeftStickPos(1).Length() > 0.1)
	{
		SpriteAnimation *sa = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER2_WALK]);
		if (sa)
		{
			sa->m_anim->animActive = true;
			sa->Update(dt);
			m_player2->SetMesh(meshList[GEO_PLAYER2_WALK]);
		}
	}
	else
	{
		m_player2->SetMesh(meshList[GEO_PLAYER2]);
	}

	if (Application::IsButtonPressed(1, Application::R2))
	{
		SpriteAnimation *sa = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER2_ATTACK]);
		if (sa)
		{
			sa->m_anim->animActive = true;
			sa->Update(dt);
			m_player2->SetMesh(meshList[GEO_PLAYER2_ATTACK]);
		}
	}

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
    {
        GameObject *go = (GameObject *)*it;
        if (go->GetActive())
        {
            go->Update(dt);
            if (go->GetType() == GameObject::SPAWNER)
            {
                dynamic_cast<Spawner*>(go)->Update(dt, m_player1, m_player2, m_currLevel->m_TerrainMap);
            }
            else if (go->GetType() == GameObject::DOOR)
            {
                dynamic_cast<Door*>(go)->Update(dt, m_currLevel->m_SpawnMap);
            }
        }
    }

    // Loop thru objects
    for (std::vector<GameObject*>::size_type i = 0; i < m_goList.size(); ++i)
    {
        GameObject* go = m_goList[i];
		if (!go->GetActive())
			continue;
        if (go->GetType() == GameObject::DOOR)
        {
            if (dynamic_cast<Door*>(go)->m_doorType == Door::UNLOCKABLE)
                continue;

            if (CheckCollisionWithPlayer(m_player1, go) && CheckCollisionWithPlayer(m_player2, go))
            {
                GoNextLevel();
            }
        }

        if (go->GetType() == GameObject::SPAWNER)
        {
            for (std::vector<GameObject*>::size_type i2 = 0; i2 < dynamic_cast<Spawner*>(go)->m_enemyList.size(); ++i2)
            {
                if (!dynamic_cast<Spawner*>(go)->m_enemyList[i2]->GetActive())
                    continue;

                for (std::vector<Projectile*>::size_type i3 = 0; i3 < dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList.size(); ++i3)
                {
                    if (!dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList[i3]->GetActive())
                        continue;

                    if (CheckCollisionWithPlayer(m_player1, dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList[i3]))
                    {
                        dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList[i3]->SetActive(false);
                        m_player1->TakeDamage(1);
                    }

                    if (CheckCollisionWithPlayer(m_player2, dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList[i3]))
                    {
                        dynamic_cast<Spawner*>(go)->m_enemyList[i2]->m_ProjectileList[i3]->SetActive(false);
                        m_player2->TakeDamage(1);
                    }
                }
            }
        }
        for (std::vector<GameObject*>::size_type i2 = 0; i2 < m_goList.size(); ++i2)
        {
            GameObject *go2 = m_goList[i2];
			if (!go2->GetActive())
				continue;
            if (go->CheckCollisionWith(go2))
            {
                Interactions(go, go2);
            }
        }
    }

    // Loop for player projectiles
    for (std::vector<Projectile*>::size_type i = 0; i < m_player1->m_ProjectileList.size(); ++i)
    {
        if (!m_player1->m_ProjectileList[i]->GetActive())
            continue;

        Projectile* go = m_player1->m_ProjectileList[i];
		if (!go->GetActive())
			continue;
        for (std::vector<GameObject*>::size_type i2 = 0; i2 < m_goList.size(); ++i2)
        {
            GameObject *go2 = m_goList[i2];
            if (!go2->GetActive())
                continue;

            if (go2->GetType() == GameObject::SPAWNER)
            {
                for (std::vector<GameObject*>::size_type i3 = 0; i3 < dynamic_cast<Spawner*>(go2)->m_enemyList.size(); ++i3)
                {
                    if (!dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->GetActive())
                        continue;

                    if (go->CheckCollisionWith(dynamic_cast<Spawner*>(go2)->m_enemyList[i3]))
                    {

                        if (go->GetType() == GameObject::PROJECTILE_MELEE && dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->GetEnemyType() == Enemy::MELEE)
                        {
                            dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->TakeDamage(dynamic_cast<Projectile*>(go)->GetDmg());
                            m_player1->points += 10;
                            go->SetActive(false);
                        }

                        if (go->GetType() == GameObject::PROJECTILE_RANGED && dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->GetEnemyType() == Enemy::RANGED)
                        {
                            dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->TakeDamage(dynamic_cast<Projectile*>(go)->GetDmg());
                            m_player1->points += 20;
                            go->SetActive(false);
                        }
                    }
    
                        //dynamic_cast<Spawner*>(go2)->m_enemyList[i3]->SetActive(false);
                   
                }
            }
            else if (go->CheckCollisionWith(go2))
            {
                Interactions(go, go2);
            }
			else if (go->CheckCollisionWithMap(m_currLevel->m_TerrainMap, m_currLevel->m_SpawnMap))
			{
				go->SetActive(false);
			}
        }
    }

    if (Application::IsButtonPressed(0, Application::CIRCLE) && !NEXTLEVELONCE)
    {
        GoNextLevel();
        NEXTLEVELONCE = true;
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

void SceneGame::RenderUI()
{
    float offset = 40;
    static float x = 0;
    static float y = 0;
    if (Application::IsKeyPressed(VK_UP))
        y += 0.3;
    if (Application::IsKeyPressed(VK_DOWN))
        y -= 0.3;
    if (Application::IsKeyPressed(VK_RIGHT))
        x += 0.3;
    if (Application::IsKeyPressed(VK_LEFT))
        x -= 0.3;
    std::cout << x << " " << y << endl;
    RenderTextOnScreen(meshList[GEO_TEXT], "Player 1 ", Color(1, 1, 1), 3, 1.2, 55);
    RenderTextOnScreen(meshList[GEO_TEXT], "Player 2 ", Color(1, 1, 1), 3, 57, 55);
    for (int i = 0; i < 5; i++)
    {
        modelStack.PushMatrix();
        modelStack.Translate(25 + i * 32, 540, 0);
        modelStack.Scale(32, 32, 32);
        RenderMesh(meshList[GEO_GREYHEART], false);
        modelStack.PopMatrix();
        if (i <= m_player1->GetHP())
        {
            modelStack.PushMatrix();
            modelStack.Translate(25 + i * 32, 540, 1);
            modelStack.Scale(32, 32, 32);
            RenderMesh(meshList[GEO_REDHEART], false);
            modelStack.PopMatrix();
        }
    }
    for (int i = 0; i < 5; i++)
    {
        modelStack.PushMatrix();
        modelStack.Translate(644 + i * 32, 540, 0);
        modelStack.Scale(32, 32, 32);
        RenderMesh(meshList[GEO_GREYHEART], false);
        modelStack.PopMatrix();
        if (i <= m_player2->GetHP())
        {
            modelStack.PushMatrix();
            modelStack.Translate(644 + i * 32, 540, 1);
            modelStack.Scale(32, 32, 32);
            RenderMesh(meshList[GEO_REDHEART], false);
            modelStack.PopMatrix();
        }
    }
    ostringstream ss;
    ss << setfill('0') << setw(8) << m_player1->points;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3, 30, 51);
}


void SceneGame::Interactions(GameObject* go1, GameObject* go2)
{
    switch (go1->GetType())
    {
    case GameObject::PROJECTILE_MELEE:
    {
        if (go2->GetType() == GameObject::SWITCH)
            dynamic_cast<Switch*>(go2)->InteractionResponse(go1);

        if (go2->GetType() == GameObject::ENEMY && dynamic_cast<Enemy*>(go2)->GetEnemyType() == Enemy::MELEE)
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
        if (go2->GetType() == GameObject::ENEMY && dynamic_cast<Enemy*>(go2)->GetEnemyType() == Enemy::RANGED)
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

bool SceneGame::CheckCollisionWithPlayer(Player* player, GameObject *go2)
{
    float distSquare = (player->GetPos() - go2->GetPos()).LengthSquared();
    float combinedRadiusSquare = (player->GetScale().x + go2->GetScale().x) * (player->GetScale().y + go2->GetScale().y);

    if (distSquare <= combinedRadiusSquare) {
        return true;
    }
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
			RenderMesh(meshList[GEO_RANGE_PROJECTILE], false);
			modelStack.PopMatrix();
			break;
		}

    case GameObject::PROJECTILE_MELEE:
    {
                                          modelStack.PushMatrix();
										  modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
                                          float angle = Math::RadianToDegree(atan2(go->GetVelocity().y, go->GetVelocity().x));
                                          modelStack.Rotate(angle - 90, 0, 0, 1);
                                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
                                          RenderMesh(meshList[GEO_SWORD_PROJECTILE_LAH], false);
                                          modelStack.PopMatrix();
                                          break;
    }

    case GameObject::ENEMY:
    {
                                          modelStack.PushMatrix();
                                          modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
                                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
                                          if (dynamic_cast<Enemy*>(go)->GetEnemyType() == Enemy::MELEE)
                                            RenderMesh(meshList[GEO_PLAYER1], false);
                                          else
                                              RenderMesh(meshList[GEO_PLAYER2], false);
                                          modelStack.PopMatrix();
                                          break;
    }

    //case GameObject::DOOR:
    //{
    //                          modelStack.PushMatrix();
    //                          modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
    //                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
    //                          RenderMesh(go->GetMesh(), false);
    //                          modelStack.PopMatrix();
    //                          break;
    //}

    //case GameObject::SWITCH:
    //{
    //                          modelStack.PushMatrix();
    //                          modelStack.Translate(go->GetPos().x, go->GetPos().y, go->GetPos().z);
    //                          modelStack.Scale(go->GetScale().x, go->GetScale().y, go->GetScale().z);
    //                          RenderMesh(go->GetMesh(), false);
    //                          modelStack.PopMatrix();
    //                          break;
    //}
    }
}

void SceneGame::RenderBackground()
{
    //modelStack.PushMatrix();
    //modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 0);
    //modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    //RenderMesh(meshList[GEO_BACKGROUND], false);
    //modelStack.PopMatrix();

}

void SceneGame::RenderPlayer()
{
	static float angle1 = 0;
	if (!m_player1->m_dir.IsZero())
	angle1 = Math::RadianToDegree(atan2(m_player1->m_dir.y, m_player1->m_dir.x));
    modelStack.PushMatrix();
	modelStack.Translate(m_player1->GetPos().x, m_player1->GetPos().y, m_player1->GetPos().z);
	modelStack.Translate(m_player1->GetScale().x * 0.5, m_player1->GetScale().y * 0.5, m_player1->GetScale().z);
	modelStack.Rotate(-angle1 - 90, 0, 0, 1);
	modelStack.Translate(-m_player1->GetScale().x * 0.5, -m_player1->GetScale().y * 0.5, m_player1->GetScale().z);
    modelStack.Scale(m_currLevel->m_TerrainMap->GetTileSize(), m_currLevel->m_TerrainMap->GetTileSize(), m_currLevel->m_TerrainMap->GetTileSize());
    RenderMesh(m_player1->GetMesh(), false);
    modelStack.PopMatrix();
	
	static float angle2 = 0;
	if (!m_player2->m_dir.IsZero())
	angle2 = Math::RadianToDegree(atan2(m_player2->m_dir.y, m_player2->m_dir.x));
	modelStack.PushMatrix();
    modelStack.Translate(m_player2->GetPos().x, m_player2->GetPos().y, m_player2->GetPos().z);
	modelStack.Translate(m_player2->GetScale().x * 0.5, m_player2->GetScale().y * 0.5, m_player2->GetScale().z);
	modelStack.Rotate(-angle2-90, 0, 0, 1);
	modelStack.Translate(-m_player2->GetScale().x * 0.5, -m_player2->GetScale().y * 0.5, m_player2->GetScale().z);
    modelStack.Scale(m_currLevel->m_TerrainMap->GetTileSize(), m_currLevel->m_TerrainMap->GetTileSize(), m_currLevel->m_TerrainMap->GetTileSize());
    RenderMesh(m_player2->GetMesh(), false);
	modelStack.PopMatrix();
}

void SceneGame::RenderRayTracing()
{
	Vector3 dir = Application::GetRightStickPos(0);
    float angle = Math::RadianToDegree(atan2(dir.y, dir.x));

    // Render ray
    glLineWidth(2.f);

	if (!dir.IsZero())
	{
		m_player1->m_dir = dir.Normalized();

		modelStack.PushMatrix();
		modelStack.Translate(m_player1->GetPos().x + m_player1->GetScale().x * 0.5, m_player1->GetPos().y + m_player1->GetScale().y * 0.5, m_player1->GetPos().z);
		modelStack.Rotate(-angle, 0, 0, 1);
		modelStack.Scale(500, 1, 1);
		RenderMesh(meshList[GEO_RAYBLUE], false);
		modelStack.PopMatrix();
	}


	dir = Application::GetRightStickPos(1);
	angle = Math::RadianToDegree(atan2(dir.y, dir.x));
	if (!dir.IsZero())
	{
		m_player2->m_dir = dir.Normalized();

		modelStack.PushMatrix();
		modelStack.Translate(m_player2->GetPos().x + m_player2->GetScale().x * 0.5, m_player2->GetPos().y + m_player2->GetScale().y * 0.5, m_player2->GetPos().z);
		modelStack.Rotate(-angle, 0, 0, 1);
		modelStack.Scale(500, 1, 1);
		RenderMesh(meshList[GEO_RAYRED], false);
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

    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "PAUSE", Color(1, 0, 1), 3, 34, 50);

    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "RESUME", Color(1, 0, 1), 3, 32, 40.5f);
    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "QUIT", Color(1, 0, 1), 3, 35, 31.5f);
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

    //if (m_lives == 0) {     //player lost
    //    RenderTextOnScreen(meshList[GEO_MENUTEXT], "YOU DIED", Color(1, 0, 1), 3, 31, 50);
    //}
    //else {      //player beat the level
    //    if (levelCount == 3) {
    //        RenderTextOnScreen(meshList[GEO_MENUTEXT], "YOU WIN!", Color(1, 0, 1), 3, 31, 50);
    //    }
    //    else {
    //        RenderTextOnScreen(meshList[GEO_MENUTEXT], "LEVEL COMPLETE", Color(1, 0, 1), 3, 22, 50);
    //    }
    //}

    //if (m_lives == 0 || levelCount < 3) {
    //    RenderTextOnScreen(meshList[GEO_MENUTEXT], "CONTINUE", Color(1, 0, 1), 3, 29, 40.5f);
    //}
    //else {
    //    RenderTextOnScreen(meshList[GEO_MENUTEXT], "MAIN MENU", Color(1, 0, 1), 2, 31.5f, 41.f);
    //}
    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "QUIT", Color(1, 0, 1), 3, 35, 31.5f);

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
	RenderTileMap(m_currLevel->m_TerrainMap);
	RenderTileMap(m_currLevel->m_SpawnMap);

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

            if (go->GetType() == GameObject::SPAWNER)
            {
                for (std::vector<Enemy*>::iterator it2 = dynamic_cast<Spawner*>(go)->m_enemyList.begin(); it2 != dynamic_cast<Spawner*>(go)->m_enemyList.end(); ++it2)
                {
                    Enemy *go2 = (Enemy *)*it2;
                    if (!go2->GetActive())
                        continue;

                    RenderGO(go2);

                    for (int i = 0; i < go2->m_Emitter->GetParticles().size(); ++i)
                    {
                        RenderParticles(go2->m_Emitter->GetParticles()[i]);
                    }
           
                    m_renderCount += 0.1f;

                    for(std::vector<Projectile*>::iterator it3 = dynamic_cast<Enemy*>(go2)->m_ProjectileList.begin(); it3 != dynamic_cast<Enemy*>(go2)->m_ProjectileList.end(); ++it3)
                    {
                        Projectile *go3 = (Projectile *)*it3;
                        if (go3->GetActive())
                        {
                            RenderGO(go3);
                            m_renderCount += 0.1f;
                        }
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
    RenderUI();
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

void SceneGame::SpawnObjects(CMap *map)
{
    typedef std::map<int, Vector3>::iterator it_type;
    for (it_type it = map->m_SpawnLocations.begin(); it != map->m_SpawnLocations.end(); ++it)
    {
        switch (it->first)
        {
        case 5: // Melee Monster
        {
                    //Enemy* enemy = new Enemy();
                    //enemy->SetActive(true);
                    //enemy->SetPos(it->second);
                    //enemy->SetScale(Vector3(15, 15, 5));
                    //enemy->SetTarget(m_player1->GetPos());
                    //enemy->SetEnemyType(Enemy::MELEE);
                    //enemy->SetMesh(meshList[GEO_PLAYER1]);
                    //enemy->SetType(GameObject::ENEMY);
                    //m_goList.push_back(enemy);

                    Spawner* spawner = new Spawner();
                    spawner->SetActive(true);
                    spawner->m_TypeToSpawn = Enemy::MELEE;
                    spawner->SetPos(it->second);
                    spawner->SetScale(Vector3(15, 15, 5));
                    spawner->SetMesh(meshList[GEO_PLAYER1]);
                    spawner->SetType(GameObject::SPAWNER);
                    m_goList.push_back(spawner);

                    break;
        }

        case 6: // Ranged Monster
        {
                    //Enemy* enemy = new Enemy();
                    //enemy->SetActive(true);
                    //enemy->SetPos(it->second);
                    //enemy->SetScale(Vector3(15, 15, 5));
                    //enemy->SetTarget(m_player1->GetPos());
                    //enemy->SetEnemyType(Enemy::RANGED);
                    //enemy->SetMesh(meshList[GEO_PLAYER1]);
                    //enemy->SetType(GameObject::ENEMY);
                    //m_goList.push_back(enemy);

                    Spawner* spawner = new Spawner();
                    spawner->SetActive(true);
                    spawner->m_TypeToSpawn = Enemy::RANGED;
                    spawner->SetPos(it->second);
                    spawner->SetScale(Vector3(15, 15, 5));
                    spawner->SetMesh(meshList[GEO_PLAYER2]);
                    spawner->SetType(GameObject::SPAWNER);
                    m_goList.push_back(spawner);
                    break;
        }

        case 97: // P1
        {
                     m_player1->SetPos(it->second);
                     break;
        }

        case 98: // P2
        {
                     m_player2->SetPos(it->second);
                     break;
        }

        case 99: // Exit door
        {
            Door* door = new Door();
            door->SetActive(true);
            door->SetPos(it->second);
            door->SetScale(Vector3(map->GetTileSize(), map->GetTileSize(), 5));
            door->SetMesh(meshList[GEO_DOOR]);
            door->SetType(GameObject::DOOR);
            door->m_doorType = Door::EXIT;
            m_goList.push_back(door);
            break;
        }
        }

        if (it->first > 50 && it->first < 80)
        {
            int spawnType = it->first / 10 % 10; // Spawn type: 5 == Melee, 6 == Ranged
            int spawnerNum = it->first % 10;  // SpawnerNum

            if (spawnType = 5)
            {
                Spawner* spawner = new Spawner();
                spawner->SetActive(true);
                spawner->m_TypeToSpawn = Enemy::MELEE;
                spawner->SetPos(it->second);
                spawner->SetScale(Vector3(15, 15, 5));
                spawner->SetMesh(meshList[GEO_PLAYER1]);
                spawner->SetType(GameObject::SPAWNER);
                m_goList.push_back(spawner);
            }
            else if (spawnType = 6)
            {
                Spawner* spawner = new Spawner();
                spawner->SetActive(true);
                spawner->m_TypeToSpawn = Enemy::RANGED;
                spawner->SetPos(it->second);
                spawner->SetScale(Vector3(15, 15, 5));
                spawner->SetMesh(meshList[GEO_PLAYER2]);
                spawner->SetType(GameObject::SPAWNER);
                m_goList.push_back(spawner);
            }
            else if (spawnType = 7)
            {
                Spawner* spawner = new Spawner();
                spawner->SetActive(true);
                spawner->m_TypeToSpawn = Enemy::NORMAL;
                spawner->SetPos(it->second);
                spawner->SetScale(Vector3(15, 15, 5));
                spawner->SetMesh(meshList[GEO_PLAYER2]);
                spawner->SetType(GameObject::SPAWNER);
                m_goList.push_back(spawner);
            }

        }

        if (it->first > 1000)
        {
            int pairNum = it->first / 1000 % 10; // Pair Number
            int objectType = it->first / 100 % 10; // Object: 1 == Door, 2 == Switch
            int doorNum = it->first / 10 % 10; // If applicable, 1 == first door, 2 == second door else 0
            int weaknessType = it->first % 10;  // Weakness: 1 == BOTH, 2 == MELEE, 3 == RANGED

            if (objectType == 1)
            {
                Door* door = new Door();
                door->SetActive(true);
                door->SetPos(it->second);
                door->SetScale(Vector3(map->GetTileSize(), map->GetTileSize(), 5));
                door->SetMesh(meshList[GEO_DOOR]);
                door->SetType(GameObject::DOOR);

                typedef std::map<int, Vector3>::iterator it_type;
                for (it_type it2 = map->m_SpawnLocations.begin(); it2 != map->m_SpawnLocations.end(); it2++)
                {
                    int switchNum = (pairNum * 1000) + 200 + weaknessType;
                    if (it2->first == switchNum)
                    {
                        Switch* temp = new Switch();
                        temp->SetActive(true);
                        temp->SetPos(it2->second);
                        temp->SetScale(Vector3(map->GetTileSize(), map->GetTileSize(), 5));
                        temp->SetMesh(meshList[GEO_PLAYER1]);
                        temp->SetType(GameObject::SWITCH);

                        if (weaknessType == 1)
                            temp->m_SwitchType = Switch::EITHER;
                        else if (weaknessType == 2)
                            temp->m_SwitchType = Switch::MELEE_ONLY;
                        else if (weaknessType == 3)
                            temp->m_SwitchType = Switch::RANGED_ONLY;

                        m_goList.push_back(temp);
                        door->m_switch = temp;
                        break;
                    }

                }
                m_goList.push_back(door);

            }
        }
    }
}

void SceneGame::GoNextLevel()
{
    Level::LEVELS temp = static_cast<Level::LEVELS>(m_currLevel->m_LevelNum + 1);

    m_goList.clear();

    if (m_currLevel)
        delete m_currLevel;
    
    m_currLevel = new Level();
    m_currLevel->Init(Application::GetWindowHeight(), Application::GetWindowWidth(), 32, temp);
    SpawnObjects(m_currLevel->m_SpawnMap);
}

void SceneGame::RenderParticles(ParticleObject* particle)
{
    switch (particle->type)
    {
    case ParticleObject_TYPE::P_NORMAL_BLOOD:
    {
        modelStack.PushMatrix();
        modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
        modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
        RenderMesh(meshList[GEO_RED_BLOOD], false);
        modelStack.PopMatrix();
        break;
    }

    case ParticleObject_TYPE::P_SILVER_BLOOD:
    {
        glBlendFunc(GL_ONE, GL_ONE);
        modelStack.PushMatrix();
        modelStack.Translate(particle->pos.x, particle->pos.y, particle->pos.z);
        modelStack.Scale(particle->scale.x, particle->scale.y, particle->scale.z);
        RenderMesh(meshList[GEO_SILVER_BLOOD], false);
        modelStack.PopMatrix();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    }
    }
}