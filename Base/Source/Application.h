#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"
#include "Scene.h"
#include <irrKlang.h>
#include "Vector3.h"
#include "SceneManager.h"

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

	enum GAMEPAD_CONTROLS
	{
		SQUARE,
		CROSS,
		CIRCLE,
		TRIANGLE,
		L1,
		R1,
		L2,
		R2,
		SHARE,
		OPTIONS,
		RIGHT_STICK,
		LEFT_STICK,
		PSBUTTON,
		TOUCH_PAD,
		DPAD_UP,
		DPAD_RIGHT,
		DPAD_DOWN,
		DPAD_LEFT,
		TOTAL_BUTTONS,
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
	static int NumOfGamepadsPresent();
	static bool IsButtonPressed(int gamepad, GAMEPAD_CONTROLS button);
	static const Vector3 GetLeftStickPos(int controllerIndex);
	static const Vector3 GetRightStickPos(int controllerIndex);

    void GetMonitorResolution();

	static int GetWindowWidth();
	static int GetWindowHeight();

    // Game Scenes
    SceneManager sceneManager;

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