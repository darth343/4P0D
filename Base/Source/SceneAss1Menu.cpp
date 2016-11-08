#include "SceneAss1Menu.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAss1Menu::SceneAss1Menu()
{
}

SceneAss1Menu::~SceneAss1Menu()
{
}

void SceneAss1Menu::Init()
{
    SceneBase::Init();

    mode = MENU_MAIN;
    help_mode = HELP_STORY;

    for (MENU_MODE i = MENU_MAIN; i < MENU_TOTAL; i = static_cast<MENU_MODE>(i + 1)) {
        button_highlighted[i] = false;
    }

    bLButtonState = false;
}

void SceneAss1Menu::Update(double dt)
{
    SceneBase::Update(dt);

    double x, y;
    Application::GetCursorPos(&x, &y);
    switch (mode)
    {
    case MENU_MAIN:
        //check which button is being selected
        for (MENU_MODE i = MENU_MAIN; i < MENU_TOTAL; i = static_cast<MENU_MODE>(i + 1)) {
            if (x > 0.35f * Application::GetWindowWidth() && x < 0.65f * Application::GetWindowWidth()
                && y > (i * 0.1f + 0.35f) * Application::GetWindowHeight() && y < (i * 0.1f + 0.42f) * Application::GetWindowHeight()) {
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
            for (MENU_MODE i = MENU_MAIN; i < MENU_TOTAL; i = static_cast<MENU_MODE>(i + 1)) {
                if (button_highlighted[i]) {    //being selected
                    Application::GetInstance().PlayButtonPressSE();

                    if (i == MENU_MAIN) {   //PLAY
                        Application::GetInstance().b_modeChange = true;
                        Application::GetInstance().mode = Application::GAMEMODE_PLAY;
                    }

                    else if (i >= MENU_INSTRUCTIONS && i <= MENU_OPTIONS) {
                        mode = i;
                    }

                    else {
                        Application::GetInstance().b_modeChange = true;
                        Application::GetInstance().mode = Application::GAMEMODE_EXIT;
                    }
                    break;
                }
            }
        }
        break;

    case MENU_INSTRUCTIONS:
        CheckBackButton();

        CheckHelpToggleButton();
        break;

    case MENU_OPTIONS:
        CheckBackButton();
        
        if (!bLButtonState && Application::IsMousePressed(0))
        {
            bLButtonState = true;
        }
        else if (bLButtonState && !Application::IsMousePressed(0))
        {
            bLButtonState = false;
            
            double x, y;
            Application::GetCursorPos(&x, &y);
            if (x > 0.3125f * Application::GetWindowWidth() && x < 0.375f * Application::GetWindowWidth()
                && y > 0.365f * Application::GetWindowHeight() && y < 0.4f * Application::GetWindowHeight()) {
                if (!Application::GetInstance().playMusic) {
                    Application::GetInstance().playMusic = true;
                    Application::GetInstance().PlayMenuBGM();
                }
                Application::GetInstance().PlayButtonPressSE();
            }

            else if (x > 0.4f * Application::GetWindowWidth() && x < 0.5f * Application::GetWindowWidth()
                && y > 0.365f * Application::GetWindowHeight() && y < 0.4f * Application::GetWindowHeight()) {
                if (Application::GetInstance().playMusic) {
                    Application::GetInstance().m_soundEngine->stopAllSounds();
                    Application::GetInstance().playMusic = false;
                }
                Application::GetInstance().PlayButtonPressSE();
            }

            else if (x > 0.3125f * Application::GetWindowWidth() && x < 0.375f * Application::GetWindowWidth()
                && y > 0.45f * Application::GetWindowHeight() && y < 0.485f * Application::GetWindowHeight()) {
                if (!Application::GetInstance().playSound) {
                    Application::GetInstance().playSound = true;
                }
                Application::GetInstance().PlayButtonPressSE();
            }
            else if (x > 0.4f * Application::GetWindowWidth() && x < 0.5f * Application::GetWindowWidth()
                && y > 0.45f * Application::GetWindowHeight() && y < 0.485f * Application::GetWindowHeight()) {
                if (Application::GetInstance().playSound) {
                    Application::GetInstance().playSound = false;
                }
            }

        }
        break;

    }

}

void SceneAss1Menu::Render()
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

    switch (mode)
    {
    case MENU_MAIN:
        RenderMainMenu();
        break;

    case MENU_INSTRUCTIONS:
        RenderInstructions();
        break;

    case MENU_OPTIONS:
        RenderOptions();
        break;
    }
}

void SceneAss1Menu::Exit()
{
    SceneBase::Exit();
}

void SceneAss1Menu::RenderMainMenu()
{
    // Logo
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.8f, 0);
    modelStack.Scale(m_worldWidth * 0.5f, m_worldHeight * 0.3f, 1.f);
    RenderMesh(meshList[GEO_LOGO], false);
    modelStack.PopMatrix();

    // Buttons
    for (MENU_MODE i = MENU_MAIN; i < MENU_TOTAL; i = static_cast<MENU_MODE>(i + 1)) {
        modelStack.PushMatrix();
        modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * (0.6f - i * 0.1f), 0);

        if (button_highlighted[i]) {
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
        modelStack.PopMatrix();
    }

    // Text on buttons
    if (button_highlighted[0]) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "PLAY", Color(1, 1, 1), 2, 0.5f * 80 - 3.5f, 0.6f * 60 - 1.f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "PLAY", Color(0.1f, 0.1f, 0.1f), 2, 0.5f * 80 - 3.5f, 0.6f * 60 - 1.f);
    }

    if (button_highlighted[1]) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "INSTRUCTIONS", Color(1, 1, 1), 2, 0.5f * 80 - 11.5f, 0.5f * 60 - 1.f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "INSTRUCTIONS", Color(0.1f, 0.1f, 0.1f), 2, 0.5f * 80 - 11.5f, 0.5f * 60 - 1.f);
    }

    if (button_highlighted[2]) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "OPTIONS", Color(1, 1, 1), 2, 0.5f * 80 - 6.5f, 0.4f * 60 - 1.f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "OPTIONS", Color(0.1f, 0.1f, 0.1f), 2, 0.5f * 80 - 6.5f, 0.4f * 60 - 1.f);
    }

    if (button_highlighted[3]) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "EXIT", Color(1, 1, 1), 2, 0.5f * 80 - 3.5f, 0.3f * 60 - 1.f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "EXIT", Color(0.1f, 0.1f, 0.1f), 2, 0.5f * 80 - 3.5f, 0.3f * 60 - 1.f);
    }

}

void SceneAss1Menu::RenderInstructions()
{
    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "INSTRUCTIONS", Color(1, 1, 0), 5, 10, 52);
    switch (help_mode) {
    case HELP_STORY:
        RenderStory();
        break;

    case HELP_CONTROLS:
        RenderControls();
        break;
        
    }

    // Back button
    RenderBackButton();

    // Toggle help button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.1f, 0);

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
    modelStack.PopMatrix();

    // Text
    if (help_mode == HELP_STORY) {
        if (button_highlighted[1]) {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "CONTROLS", Color(1, 1, 1), 2, 0.2f * 80 - 7.5f, 0.1f * 60 - 1.f);
        }
        else {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "CONTROLS", Color(0.1f, 0.1f, 0.1f), 2, 0.2f * 80 - 7.5f, 0.1f * 60 - 1.f);
        }
    }
    else if (help_mode == HELP_CONTROLS) {
        if (button_highlighted[1]) {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "STORY", Color(1, 1, 1), 2, 0.2f * 80 - 5.f, 0.1f * 60 - 1.f);
        }
        else {
            RenderTextOnScreen(meshList[GEO_MENUTEXT], "STORY", Color(0.1f, 0.1f, 0.1f), 2, 0.2f * 80 - 5.f, 0.1f * 60 - 1.f);
        }
    }
}

void SceneAss1Menu::RenderStory()
{
    RenderTextOnScreen(meshList[GEO_MENUTEXT], "STORY", Color(1, 1, 0), 5, 30, 52);

    // convo character profiles
    // C3PO
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.75f, 0);
    modelStack.Scale(m_worldWidth * 0.0375f, m_worldHeight * 0.075f, 0);
    RenderMesh(meshList[GEO_C3PO], false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.45f, 0);
    modelStack.Scale(m_worldWidth * 0.0375f, m_worldHeight * 0.075f, 0);
    RenderMesh(meshList[GEO_C3PO], false);
    modelStack.PopMatrix();

    // BOBA FETT
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.8f, m_worldHeight * 0.6f, 0);
    modelStack.Scale(m_worldWidth * 0.0375f, m_worldHeight * 0.075f, 0);
    RenderMesh(meshList[GEO_BOBAFETT], false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.8f, m_worldHeight * 0.3f, 0);
    modelStack.Scale(m_worldWidth * 0.0375f, m_worldHeight * 0.075f, 0);
    RenderMesh(meshList[GEO_BOBAFETT], false);
    modelStack.PopMatrix();

    RenderTextOnScreen(meshList[GEO_TEXT], "Sir! Sir! This is terrible!", Color(0, 1, 1), 2, 20, 45);
    RenderTextOnScreen(meshList[GEO_TEXT], "What's the matter, 3-CPU?", Color(1, 0, 1), 2, 11, 36.25f);
    RenderTextOnScreen(meshList[GEO_TEXT], "We're under attack!", Color(0, 1, 1), 2, 20, 27.5f);
    RenderTextOnScreen(meshList[GEO_TEXT], "Not to worry. I will", Color(1, 0, 1), 2, 20, 18.75f);
    RenderTextOnScreen(meshList[GEO_TEXT], "get us out of here.", Color(1, 0, 1), 2, 20, 16.75f);
}

void SceneAss1Menu::RenderControls()
{
    RenderTextOnScreen(meshList[GEO_MENUTEXT], "CONTROLS", Color(1, 1, 0), 5, 22.5f, 52);

    // Render GameObjects
    //player ship
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.75f, 0);
    modelStack.Scale(m_worldWidth * 0.025f, m_worldHeight * 0.025f, 0);
    RenderMesh(meshList[GEO_SHIP], false);
    modelStack.PopMatrix();

    //bullet
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.6f, 0);
    modelStack.Scale(m_worldWidth * 0.025f, m_worldHeight * 0.025f, 0);
    RenderMesh(meshList[GEO_BULLET], false);
    modelStack.PopMatrix();

    //missile
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.45f, 0);
    modelStack.Scale(m_worldWidth * 0.025f, m_worldHeight * 0.025f, 0);
    RenderMesh(meshList[GEO_MISSILE], false);
    modelStack.PopMatrix();

    //black hole
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.2f, m_worldHeight * 0.3f, 0);
    modelStack.Scale(m_worldWidth * 0.025f, m_worldHeight * 0.025f, 0);
    RenderMesh(meshList[GEO_BLACKHOLE], false);
    modelStack.PopMatrix();

    // Text
    RenderTextOnScreen(meshList[GEO_TEXT], "WASD to move", Color(1, 1, 1), 2, 20, 44);
    RenderTextOnScreen(meshList[GEO_TEXT], "Spacebar to shoot bullet", Color(1, 1, 1), 2, 20, 35);
    RenderTextOnScreen(meshList[GEO_TEXT], "R to shoot missile", Color(1, 1, 1), 2, 20, 26);
    RenderTextOnScreen(meshList[GEO_TEXT], "E to activate black hole", Color(1, 1, 1), 2, 20, 17);
    RenderTextOnScreen(meshList[GEO_TEXT], "Left click to place", Color(1, 1, 1), 2, 18, 14);
    RenderTextOnScreen(meshList[GEO_TEXT], "Right click to cancel", Color(1, 1, 1), 2, 18, 12);
}

void SceneAss1Menu::RenderOptions()
{
    RenderTextOnScreen(meshList[GEO_MENUTEXT], "OPTIONS", Color(1, 1, 0), 5, 25, 52);

    RenderTextOnScreen(meshList[GEO_TEXT], "Music", Color(0.5f, 0.5f, 1), 3, 6, 35);
    if (Application::GetInstance().playMusic) {
        RenderTextOnScreen(meshList[GEO_TEXT], "On", Color(1, 1, 1), 3, 25, 35);
        RenderTextOnScreen(meshList[GEO_TEXT], "Off", Color(0.5f, 0.5f, 1), 3, 33, 35);
    }
    else {
        RenderTextOnScreen(meshList[GEO_TEXT], "On", Color(0.5f, 0.5f, 1), 3, 25, 35);
        RenderTextOnScreen(meshList[GEO_TEXT], "Off", Color(1, 1, 1), 3, 33, 35);
    }

    RenderTextOnScreen(meshList[GEO_TEXT], "Sound", Color(0.5f, 0.5f, 1), 3, 6, 30);
    if (Application::GetInstance().playSound) {
        RenderTextOnScreen(meshList[GEO_TEXT], "On", Color(1, 1, 1), 3, 25, 30);
        RenderTextOnScreen(meshList[GEO_TEXT], "Off", Color(0.5f, 0.5f, 1), 3, 33, 30);
    }
    else {
        RenderTextOnScreen(meshList[GEO_TEXT], "On", Color(0.5f, 0.5f, 1), 3, 25, 30);
        RenderTextOnScreen(meshList[GEO_TEXT], "Off", Color(1, 1, 1), 3, 33, 30);
    }

    RenderBackButton();
}

void SceneAss1Menu::CheckBackButton()
{
    double x, y;
    Application::GetCursorPos(&x, &y);

    if (x > 0.65f * Application::GetWindowWidth() && x < 0.95f * Application::GetWindowWidth()
        && y > 0.85f * Application::GetWindowHeight() && y < 0.92f * Application::GetWindowHeight()) {
        button_highlighted[0] = true;
    }
    else {
        button_highlighted[0] = false;
    }

    if (button_highlighted[0]) {
        //Mouse Section
        if (!bLButtonState && Application::IsMousePressed(0))
        {
            bLButtonState = true;
        }
        else if (bLButtonState && !Application::IsMousePressed(0))
        {
            bLButtonState = false;

            if (button_highlighted[0]) {    //being selected
                Application::GetInstance().PlayButtonPressSE();
                mode = MENU_MAIN;
            }
        }
    }
}

void SceneAss1Menu::CheckHelpToggleButton()
{
    double x, y;
    Application::GetCursorPos(&x, &y);

    if (x > 0.05f * Application::GetWindowWidth() && x < 0.35f * Application::GetWindowWidth()
        && y > 0.85f * Application::GetWindowHeight() && y < 0.92f * Application::GetWindowHeight()) {
        button_highlighted[1] = true;
    }
    else {
        button_highlighted[1] = false;
    }

    if (button_highlighted[1]) {
        //Mouse Section
        if (!bLButtonState && Application::IsMousePressed(0))
        {
            bLButtonState = true;
        }
        else if (bLButtonState && !Application::IsMousePressed(0))
        {
            bLButtonState = false;

            if (button_highlighted[1]) {    //being selected
                Application::GetInstance().PlayButtonPressSE();
                if (help_mode == HELP_STORY) {
                    help_mode = HELP_CONTROLS;
                }
                else if (help_mode == HELP_CONTROLS) {
                    help_mode = HELP_STORY;
                }
            }
        }
    }
}

void SceneAss1Menu::RenderBackButton()
{
    // Button
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.8f, m_worldHeight * 0.1f, 0);

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
    modelStack.PopMatrix();

    // Text
    if (button_highlighted[0]) {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "BACK", Color(1, 1, 1), 2, 0.8f * 80 - 3.5f, 0.1f * 60 - 1.f);
    }
    else {
        RenderTextOnScreen(meshList[GEO_MENUTEXT], "BACK", Color(0.1f, 0.1f, 0.1f), 2, 0.8f * 80 - 3.5f, 0.1f * 60 - 1.f);
    }
}