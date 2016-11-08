#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <string>
#include <map>
#include "Scene.h"

class SceneManager
{
    std::map<std::string, Scene*> m_sceneList;

    Scene* m_currScene;

public:
    SceneManager();
    ~SceneManager();

    void AddScene(std::string sceneName, Scene* scene);
    void ChangeScene(std::string scene);

    void Update(double dt);
    void Render();
    void Exit();
    void TrueExit();
};

#endif