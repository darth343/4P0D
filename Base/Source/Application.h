#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "Scene.h"
#include <irrKlang.h>

using namespace irrklang;

class Application
{
public:
    enum GAME_MODE
    {
        GAMEMODE_MENU,
        GAMEMODE_PLAY,
        GAMEMODE_EXIT,
        GAMEMODE_TOTALs
    };

	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);
	static bool IsMousePressed(unsigned short key);
	static void GetCursorPos(double *xpos, double *ypos);

    void GetMonitorResolution();

	static int GetWindowWidth();
	static int GetWindowHeight();

    // Game Scenes
    Scene *scene;
    GAME_MODE mode;
    bool b_modeChange;

    // Sound
    ISoundEngine* m_soundEngine;
    bool playSound;
    bool playMusic;
    void PlayMenuBGM();
    void PlayGameBGM();
    void PlayButtonPressSE();
    void PlayBulletFireSE();
    void PlayPickupSE();
    void PlayExplosionSE();
    void PlayMissileFireSE();

private:
	Application();
	~Application();

	//Declare a window object
	StopWatch m_timer;
};

#endif