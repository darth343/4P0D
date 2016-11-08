#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include "GameObject.h"
#include "SceneBase.h"

class SceneMenu : public SceneBase
{
    enum MENU_MODE
    {
        MENU_MAIN,
        MENU_INSTRUCTIONS,
        MENU_OPTIONS,
        MENU_EXIT,
        MENU_TOTAL
    };

    enum HELP_MODE
    {
        HELP_STORY,
        HELP_CONTROLS,
        HELP_TOTAL
    };

public:
    SceneMenu();
    ~SceneMenu();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

protected:
    MENU_MODE mode;
    HELP_MODE help_mode;
    bool button_highlighted[MENU_TOTAL];

    bool bLButtonState;

    float m_worldHeight;
    float m_worldWidth;
    
    // Render
    void RenderMainMenu();
    void RenderInstructions();
    void RenderStory();
    void RenderControls();
    void RenderOptions();

    void CheckBackButton();
    void RenderBackButton();
    void CheckHelpToggleButton();
};

#endif