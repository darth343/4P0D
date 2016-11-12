#include "SceneMenu.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneMenu::SceneMenu()
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
    //Calculating aspect ratio
    m_worldHeight = 100.f;
    m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

    Application::GetInstance().PlayMenuBGM();

    SceneBase::Init();

    mode = MENU_MAIN;

    button_index = 0;
    d_timeCount = 0.0;
    d_bufferTime = 0.2;
    m_background_translation = 0.f;
    m_highlighter_translation = 0.f;
    b_translateHighlighter = false;
}

void SceneMenu::Update(double dt)
{
    SceneBase::Update(dt);

    d_timeCount += dt;

    if (b_translateHighlighter)
    {
        m_highlighter_translation += (float)(dt);
    }

    m_background_translation += 10.f * (float)(dt);
    if (m_background_translation >= m_worldWidth)
        m_background_translation -= m_worldWidth;

    switch (mode)
    {
    case MENU_MAIN:
        UpdateMainMenu();
        break;

    case MENU_INSTRUCTIONS:
        CheckBackButton();
        break;

    case MENU_OPTIONS:
        CheckBackButton();
        break;

    }

}

void SceneMenu::UpdateMainMenu()
{
    if (b_translateHighlighter && m_highlighter_translation > 0.3f)
    {
        m_highlighter_translation = 0.f;

        switch (button_index)
        {
        case MENU_MAIN:
            Application::sceneManager.toChangeScene = "Game";
            break;

        case MENU_EXIT:
            Application::sceneManager.b_toExit = true;
            break;

        default:
            mode = static_cast<MENU_MODE>(button_index);
            d_timeCount = 0.0;
            break;
        }

        b_translateHighlighter = false;
        d_timeCount = 0.0;
        button_index = 0;
    }

    else if (d_timeCount > d_bufferTime)
    {
        // Toggle up
        if (Application::IsButtonPressed(0, Application::DPAD_UP))
        {
            if (button_index > 0)
            {
                Application::GetInstance().PlayButtonToggleSE();
                --button_index;
                d_timeCount = 0.0;
            }
        }

        // Toggle down
		else if (Application::IsButtonPressed(0, Application::DPAD_DOWN))
        {
            if (button_index < MENU_TOTAL - 1)
            {
                Application::GetInstance().PlayButtonToggleSE();
                ++button_index;
                d_timeCount = 0.0;
            }
        }

        // Select
		else if (Application::IsButtonPressed(0, Application::CROSS) || Application::IsKeyPressed(VK_RETURN))
        {
            Application::GetInstance().PlayButtonPressSE();
            b_translateHighlighter = true;
        }
    }
}

void SceneMenu::Render()
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

    RenderBackground();

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

    double x, y;
    Application::GetCursorPos(&x, &y);

    stringstream ss;
    ss << "X:" << x;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3.f, 5, 10);
    ss.str("");
    ss << "Y:" << y;
    RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(1, 1, 1), 3.f, 5, 7);
}

void SceneMenu::Exit()
{
    SceneBase::Exit();
}

void SceneMenu::RenderBackground()
{
    // background
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 1.5f - m_background_translation, m_worldHeight * 0.5f, -1);

    modelStack.PushMatrix();
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_MENU_BACKGROUND], false);
    modelStack.PopMatrix();

    modelStack.PushMatrix();
    modelStack.Translate(-m_worldWidth, 0, 0);
    modelStack.Scale(m_worldWidth, m_worldHeight, 1.f);
    RenderMesh(meshList[GEO_MENU_BACKGROUND], false);
    modelStack.PopMatrix();

    modelStack.PopMatrix();
}

void SceneMenu::RenderHighlight()
{
    switch (mode)
    {
    case MENU_MAIN:
        // Sword
        modelStack.PushMatrix();
        modelStack.Translate((0.2f + m_highlighter_translation) * m_worldWidth, (0.58f - button_index * 0.15f) * m_worldHeight, 0.5f);
        modelStack.Scale(0.4f * m_worldWidth, 0.5f * m_worldHeight, 1.f);
        modelStack.Rotate(-90.f, 0, 0, 1);
        RenderMesh(meshList[GEO_MENU_HIGHLIGHT], false);
        modelStack.PopMatrix();
        
        // Staff
        modelStack.PushMatrix();
        modelStack.Translate((0.8f - m_highlighter_translation) * m_worldWidth, (0.58f - button_index * 0.15f) * m_worldHeight, 0.5f);
        modelStack.Scale(0.4f * m_worldWidth, 0.5f * m_worldHeight, 1.f);
        modelStack.Rotate(90.f, 0, 0, 1);
        RenderMesh(meshList[GEO_MENU_HIGHLIGHT2], false);
        modelStack.PopMatrix();
        break;
    }
}

void SceneMenu::RenderMainMenu()
{   
    RenderHighlight();
    
    // overlay menu buttons
    modelStack.PushMatrix();
    modelStack.Translate(m_worldWidth * 0.5f, m_worldHeight * 0.5f, 1.f);
    modelStack.Scale(m_worldWidth, m_worldHeight, 1);
    RenderMesh(meshList[GEO_MENU_FRONT], false);
    modelStack.PopMatrix();
}

void SceneMenu::RenderInstructions()
{
    //RenderTextOnScreen(meshList[GEO_MENUTEXT], "INSTRUCTIONS", Color(1, 1, 0), 5, 10, 52);

    // Highlight button

}

void SceneMenu::RenderOptions()
{
    RenderTextOnScreen(meshList[GEO_TEXT], "OPTIONS", Color(1, 1, 0), 5, 25, 52);

    RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Music", Color(0.5f, 0.5f, 1), 3, 6, 35);
    if (Application::GetInstance().playMusic) {
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "On", Color(1, 1, 1), 3, 25, 35);
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Off", Color(0.5f, 0.5f, 1), 3, 33, 35);
    }
    else {
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "On", Color(0.5f, 0.5f, 1), 3, 25, 35);
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Off", Color(1, 1, 1), 3, 33, 35);
    }

    RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Sound", Color(0.5f, 0.5f, 1), 3, 6, 30);
    if (Application::GetInstance().playSound) {
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "On", Color(1, 1, 1), 3, 25, 30);
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Off", Color(0.5f, 0.5f, 1), 3, 33, 30);
    }
    else {
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "On", Color(0.5f, 0.5f, 1), 3, 25, 30);
        RenderTextOnScreen(meshList[GEO_CALIBRITEXT], "Off", Color(1, 1, 1), 3, 33, 30);
    }

}

void SceneMenu::CheckBackButton()
{
    if (b_translateHighlighter && m_highlighter_translation > 0.3f)
    {
        m_highlighter_translation = 0.f;

        mode = static_cast<MENU_MODE>(button_index);
        d_timeCount = 0.0;

        b_translateHighlighter = false;
        d_timeCount = 0.0;
        button_index = 0;
    }

    else if (d_timeCount > d_bufferTime)
    {
        // Select
        if (Application::IsButtonPressed(0, Application::CIRCLE))
        {
            Application::GetInstance().PlayButtonPressSE();
            b_translateHighlighter = true;
        }
    }
}