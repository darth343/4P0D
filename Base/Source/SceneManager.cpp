#include "SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
    std::map<std::string, Scene*>::iterator it = m_sceneList.begin();
    while (it != m_sceneList.end())
    {
        delete it->second;
        ++it;
    }

    m_sceneList.clear();
}

void SceneManager::AddScene(std::string sceneName, Scene* scene)
{
    m_sceneList.insert(std::pair<std::string, Scene*>(sceneName, scene));
}

void SceneManager::ChangeScene(std::string sceneName)
{
    std::map<std::string, Scene*>::iterator it = m_sceneList.find(sceneName);
    m_currScene = it->second;
    m_currScene->Init();
}

void SceneManager::Update(double dt)
{
    m_currScene->Update(dt);
}

void SceneManager::Render()
{
    m_currScene->Render();
}

void SceneManager::Exit()
{
    m_currScene->Exit();
}

void SceneManager::TrueExit()
{
    m_currScene->TrueExit();
}