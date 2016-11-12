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

public:
    SceneMenu();
    ~SceneMenu();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

protected:
    MENU_MODE mode;
    unsigned int button_index;   // which button being highlighted
    double d_timeCount;     // for key input
    double d_bufferTime;    // for key input

    float m_worldHeight;
    float m_worldWidth;

    // Update
    void UpdateMainMenu();
    void CheckBackButton();
    float m_background_translation;
    float m_highlighter_translation;
    bool b_translateHighlighter;

    // Render
    void RenderBackground();
    void RenderHighlight();

    void RenderMainMenu();
    void RenderInstructions();
    void RenderOptions();
};

#endif