#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Scene.h"
#include "Mtx44.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "GameObject.h"
#include <vector>
#include "Map.h"
#include "Player.h"
#include "SpriteAnimation.h"

class SceneBase : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_TRANSPARENCY,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	enum GEOMETRY_TYPE
	{
        // menu
        GEO_LOGO,
        GEO_BUTTON_NORMAL,
        GEO_BUTTON_HIGHLIGHTED,
        GEO_TEXT,
        GEO_MENU_FRONT,
        GEO_MENU_BACKGROUND,
        GEO_MENU_HIGHLIGHT,
        GEO_MENU_HIGHLIGHT2,

        GEO_CALIBRITEXT,

        // gameplay
		GEO_FLOOR,
		GEO_WALL,
		GEO_DOOR,
		GEO_PLAYER1,
		GEO_PLAYER2,
		GEO_SPECTRE,
		GEO_ARMORENEMY,
		GEO_PLAYER1_WALK,
		GEO_PLAYER2_WALK,
		GEO_SWORD_PROJECTILE_LAH,
		GEO_RANGE_PROJECTILE,
		GEO_PLAYER1_ATTACK,
		GEO_PLAYER2_ATTACK,
        GEO_RAY,
        GEO_MELEE_SWITCH,
        GEO_RANGED_SWITCH,

        GEO_RED_BLOOD,
        GEO_SILVER_BLOOD,
        GEO_GREEN_BLOOD,
		NUM_GEOMETRY,
	};
public:
	SceneBase();
	~SceneBase();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

    virtual void TrueExit();
	void RenderTile(Mesh* mesh, float size, float x, float y);
	void RenderTileMap(CMap* map);
	void RenderFogMap(CMap* map);
	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y);
	void RenderMesh(Mesh *mesh, bool enableLight, float Opacity = 1.f);
	void RenderGO(GameObject *go);

	GameObject* FetchGO();
protected:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera camera;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[1];

	bool bLightEnabled;

	float fps;
};

#endif