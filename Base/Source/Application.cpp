#include "Application.h"

//Include GLEW
#include <GL/glew.h>

//Include GLFW
#include <GLFW/glfw3.h>

//Include the standard C++ headers
#include <stdio.h>
#include <stdlib.h>

#include "SceneBase.h"
#include "SceneGame.h"
#include "SceneMenu.h"

GLFWwindow* m_window;
const unsigned char FPS = 60; // FPS of this game
const unsigned int frameTime = 1000 / FPS; // time for each frame
int m_width, m_height;

SceneManager Application::sceneManager;

//Define an error callback
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

//Define the key input callback
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


void resize_callback(GLFWwindow* window, int w, int h)
{
	m_width = w;
	m_height = h;
	glViewport(0, 0, w, h);
}

bool Application::IsKeyPressed(unsigned short key)
{
    //return ((GetAsyncKeyState(key) & 0x8001) != 0);
    return ((GetAsyncKeyState(key) & 0x8000) != 0);
}
bool Application::IsMousePressed(unsigned short key) //0 - Left, 1 - Right, 2 - Middle
{
	return glfwGetMouseButton(m_window, key) != 0;
}
void Application::GetCursorPos(double *xpos, double *ypos)
{
	glfwGetCursorPos(m_window, xpos, ypos);
}

void Application::GetMonitorResolution()
{
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    m_width = mode->width;
    m_height = mode->height;
}

int Application::GetWindowWidth()
{
	return m_width;
}
int Application::GetWindowHeight()
{
	return m_height;
}

Application::Application()
{
    // Initialise variables

    // Sound
    m_soundEngine = createIrrKlangDevice();
    playSound = true;
    playMusic = true;
}

Application::~Application()
{
}

const Vector3 Application::GetLeftStickPos(int controllerIndex)
{
	if (!glfwJoystickPresent(controllerIndex))
		return Vector3();
	int AxesCount;
	const float* Axes = glfwGetJoystickAxes(controllerIndex, &AxesCount);
	Vector3 LeftStick(Axes[0], Axes[1]);
	if (abs(LeftStick.x) < 0.08 && abs(LeftStick.y) < 0.08)
		return Vector3();
	else
		return LeftStick;
}

const Vector3 Application::GetRightStickPos(int controllerIndex)
{
	if (!glfwJoystickPresent(controllerIndex))
		return Vector3();
	int AxesCount;
	const float* Axes = glfwGetJoystickAxes(controllerIndex, &AxesCount);
	Vector3 RightStick(Axes[2], Axes[3]);
	if (abs(RightStick.x) < 0.08 && abs(RightStick.y) < 0.08)
		return Vector3();
	else
		return RightStick;
}

bool Application::IsButtonPressed(int gamepad, GAMEPAD_CONTROLS button)
{
    if (!glfwJoystickPresent(gamepad))
        return false;
	int buttonCount;
	const unsigned char *buttons = glfwGetJoystickButtons(gamepad, &buttonCount);

	return buttons[button];
}
int Application::NumOfGamepadsPresent()
{
	int count = 0;
	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
		count++;
	if (glfwJoystickPresent(GLFW_JOYSTICK_2))
		count++;
	return count;
}


void Application::Init()
{
	//Set the error callback
	glfwSetErrorCallback(error_callback);

	//Initialize GLFW
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional
	glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 


	//Create a window and create its OpenGL context
	//m_width = 1920;
	//m_height = 1080;
    GetMonitorResolution();
	m_window = glfwCreateWindow(m_width, m_height, "The Weapon Chronicle", NULL, NULL);

	//If the window couldn't be created
	if (!m_window)
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(m_window);

	//Sets the key callback
	//glfwSetKeyCallback(m_window, key_callback);
	glfwSetWindowSizeCallback(m_window, resize_callback);

	glewExperimental = true; // Needed for core profile
	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) 
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		//return -1;
	}
}

void Application::Run()
{
    sceneManager.AddScene("Menu", new SceneMenu());
    sceneManager.AddScene("Game", new SceneGame());
	sceneManager.ChangeScene("Menu");

    //Main Loop
	m_timer.startTimer();    // Start timer to calculate how long it takes to render this frame
	
    while (!glfwWindowShouldClose(m_window) && !sceneManager.b_toExit && !IsKeyPressed(VK_ESCAPE))
	{
		sceneManager.Update(m_timer.getElapsedTime());
		sceneManager.Render();
		//Swap buffers
		glfwSwapBuffers(m_window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...
		glfwPollEvents();
        m_timer.waitUntil(frameTime);       // Frame rate limiter. Limits each frame to a specified time in ms.
	} //Check if the game has been exited the window had been closed

    sceneManager.Exit();
    sceneManager.TrueExit();
}

void Application::Exit()
{
    // delete irrKlang engine
    m_soundEngine->drop();

	//Close OpenGL window and terminate GLFW
	glfwDestroyWindow(m_window);
	//Finalize and clean up GLFW
	glfwTerminate();
}

void Application::StopAllSounds()
{
    m_soundEngine->stopAllSounds();
}

void Application::PlayMenuBGM()
{
    if (playMusic) {
        m_soundEngine->play2D("Sound//MainMenu BGM.mp3", true);
    }
}

void Application::PlayGameBGM()
{
    if (playMusic) {
        m_soundEngine->play2D("Sound//Dungeon BGM.mp3", true);
    }
}

void Application::PlayButtonToggleSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//button_toggle.wav");
    }
}

void Application::PlayButtonPressSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//button_select.wav");
    }
}

void Application::PlayBulletFireSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//SE_bullet fire.wav");
    }
}

void Application::PlayPickupSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//SE_pickup.wav");
    }
}

void Application::PlayExplosionSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//SE_explosion.mp3");
    }
}

void Application::PlayMissileFireSE()
{
    if (playSound) {
        m_soundEngine->play2D("Sound//SE_missile fire.wav");
    }
}