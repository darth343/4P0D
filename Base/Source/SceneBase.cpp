#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>

SceneBase::SceneBase()
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 
	
	glEnable(GL_CULL_FACE);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders( "Shader//comg.vertexshader", "Shader//comg.fragmentshader" );
	
	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_TRANSPARENCY] = glGetUniformLocation(m_programID, "transparency");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");
	
	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 20, 0);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);
	
	glUniform1i(m_parameters[U_NUMLIGHTS], 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));

	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
    meshList[GEO_LOGO] = MeshBuilder::GenerateQuad("logo", Color(1, 1, 1), 1.f);
    meshList[GEO_LOGO]->textureID = LoadTGA("Image//logo.tga");

    meshList[GEO_BUTTON_NORMAL] = MeshBuilder::GenerateQuad("button", Color(1, 0, 1), 1.f);
    meshList[GEO_BUTTON_NORMAL]->textureID = LoadTGA("Image//button_normal.tga");

    meshList[GEO_BUTTON_HIGHLIGHTED] = MeshBuilder::GenerateQuad("button highlighted", Color(0, 0, 1), 1.f);
    meshList[GEO_BUTTON_HIGHLIGHTED]->textureID = LoadTGA("Image//button_highlighted.tga");

    meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
    meshList[GEO_TEXT]->textureID = LoadTGA("Image//Fonts/Font_Dungeon.tga");
    meshList[GEO_TEXT]->LoadFontData("Image//Fonts/FontData_Dungeon.csv");
    meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);

    meshList[GEO_CALIBRITEXT] = MeshBuilder::GenerateText("text", 16, 16);
    meshList[GEO_CALIBRITEXT]->textureID = LoadTGA("Image//Fonts/calibri.tga");
    meshList[GEO_CALIBRITEXT]->material.kAmbient.Set(1, 0, 0);

    meshList[GEO_MENU_FRONT] = MeshBuilder::GenerateQuad("menu front", Color(1, 1, 1), 1.f);
    meshList[GEO_MENU_FRONT]->textureID = LoadTGA("Image//menu_front.tga");

    meshList[GEO_MENU_BACKGROUND] = MeshBuilder::GenerateQuad("menu background", Color(0, 0, 0), 1.f);
    meshList[GEO_MENU_BACKGROUND]->textureID = LoadTGA("Image//menu_background.tga");

    meshList[GEO_MENU_HIGHLIGHT] = MeshBuilder::GenerateQuad("menu button highlight", Color(0, 0, 0), 1.f);
    meshList[GEO_MENU_HIGHLIGHT]->textureID = LoadTGA("Image//Sword.tga");

    meshList[GEO_MENU_HIGHLIGHT2] = MeshBuilder::GenerateQuad("menu button highlight 2", Color(0, 0, 0), 1.f);
    meshList[GEO_MENU_HIGHLIGHT2]->textureID = LoadTGA("Image//Staff.tga");

	meshList[GEO_FLOOR] = MeshBuilder::GenerateTile("floor", Color(0, 0, 0), 1.f);
	meshList[GEO_FLOOR]->textureID = LoadTGA("Image//Ground.tga");

	meshList[GEO_WALL] = MeshBuilder::GenerateTile("wall", Color(0, 0, 0), 1.f);
	meshList[GEO_WALL]->textureID = LoadTGA("Image//Wall.tga");

	meshList[GEO_DOOR] = MeshBuilder::GenerateTile("door", Color(0, 0, 0), 1.f);
	meshList[GEO_DOOR]->textureID = LoadTGA("Image//Door.tga");

	meshList[GEO_PLAYER1] = MeshBuilder::GenerateTile("player", Color(0, 1, 0), 1.f);
    meshList[GEO_PLAYER1]->textureID = LoadTGA("Image//Sword 2.tga");
	meshList[GEO_PLAYER2] = MeshBuilder::GenerateTile("player 2", Color(0, 1, 0), 1.f);
	meshList[GEO_PLAYER2]->textureID = LoadTGA("Image//Staff.tga");
	meshList[GEO_ARMORENEMY] = MeshBuilder::GenerateTile("Armor Enemy", Color(0, 1, 0), 1.f);
	meshList[GEO_ARMORENEMY]->textureID = LoadTGA("Image//ArmorEnemy.tga");
	meshList[GEO_SPECTRE] = MeshBuilder::GenerateTile("GEO_SPECTRE", Color(0, 1, 0), 1.f);
	meshList[GEO_SPECTRE]->textureID = LoadTGA("Image//Spectre.tga");
    meshList[GEO_RAY] = MeshBuilder::GenerateLine("line", Color(0, 0, 1), 1.f);

	meshList[GEO_SWORD_PROJECTILE_LAH] = MeshBuilder::GenerateTile("GEO_SWORD_PROJECTILE_LAH", Color(0, 1, 0), 1.f);
	meshList[GEO_SWORD_PROJECTILE_LAH]->textureID = LoadTGA("Image//MeleeProjectile.tga");

	meshList[GEO_RANGE_PROJECTILE] = MeshBuilder::GenerateTile("GEO_RANGE_PROJECTILE", Color(0, 1, 0), 1.f);
	meshList[GEO_RANGE_PROJECTILE]->textureID = LoadTGA("Image//Projectile1.tga");

	meshList[GEO_PLAYER1_WALK] = MeshBuilder::GenerateSpriteAnimation("Player 1 walk", 1, 3);
	meshList[GEO_PLAYER1_WALK]->textureID = LoadTGA("Image//Sword_Idle.tga");

	meshList[GEO_PLAYER1_ATTACK] = MeshBuilder::GenerateSpriteAnimation("GEO_PLAYER1_ATTACK", 1, 3);
	meshList[GEO_PLAYER1_ATTACK]->textureID = LoadTGA("Image//Sword_Attacking.tga");

	meshList[GEO_PLAYER2_WALK] = MeshBuilder::GenerateSpriteAnimation("GEO_PLAYER2_WALK", 1, 3);
	meshList[GEO_PLAYER2_WALK]->textureID = LoadTGA("Image//Staff_Idle.tga");
					   
	meshList[GEO_PLAYER2_ATTACK] = MeshBuilder::GenerateSpriteAnimation("GEO_PLAYER2_ATTACK", 1, 3);
	meshList[GEO_PLAYER2_ATTACK]->textureID = LoadTGA("Image//Staff_Attack.tga");

    meshList[GEO_RED_BLOOD] = MeshBuilder::GenerateSpriteAnimation("GEO_RED_BLOOD", 1, 3);
    meshList[GEO_RED_BLOOD]->textureID = LoadTGA("Image//ArmoredBlood.tga");

    meshList[GEO_GREEN_BLOOD] = MeshBuilder::GenerateSpriteAnimation("GEO_GREEN_BLOOD", 1, 3);
    meshList[GEO_GREEN_BLOOD]->textureID = LoadTGA("Image//SlimeBlood.tga");

    meshList[GEO_SILVER_BLOOD] = MeshBuilder::GenerateSpriteAnimation("GEO_SILVER_BLOOD", 1, 3);
    meshList[GEO_SILVER_BLOOD]->textureID = LoadTGA("Image//GhostSmoke.tga");

	SpriteAnimation *player1walk = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER1_WALK]);
	if (player1walk)
	{
		player1walk->m_anim = new Animation();
		player1walk->m_anim->Set(0, 2, 1, 0.8f, true);
	}

	SpriteAnimation *player1attack = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER1_ATTACK]);
	if (player1attack)
	{
		player1attack->m_anim = new Animation();
		player1attack->m_anim->Set(0, 2, 1, 0.2f, true);
	}

	SpriteAnimation *player2walk = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER2_WALK]);
	if (player2walk)
	{
		player2walk->m_anim = new Animation();
		player2walk->m_anim->Set(0, 2, 1, 0.8f, true);
	}

	SpriteAnimation *player2attack = dynamic_cast<SpriteAnimation*>(meshList[GEO_PLAYER2_ATTACK]);
	if (player2attack)
	{
		player2attack->m_anim = new Animation();
		player2attack->m_anim->Set(0, 2, 1, 0.2f, true);
	}

    meshList[GEO_DOOR] = MeshBuilder::GenerateTile("door", Color(0, 1, 0), 1.f);
    meshList[GEO_DOOR]->textureID = LoadTGA("Image//Door.tga");

    meshList[GEO_RAY] = MeshBuilder::GenerateLine("line", Color(1, 0, 0), 1.f);

    bLightEnabled = false;
}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	fps = (float)(1.f / dt);
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTile(Mesh* mesh, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0)
		return;
	Mtx44 ortho;
	ortho.SetToOrtho(0, 800, 0, 600, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, 1);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	mesh->Render();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
}

void SceneBase::RenderTileMap(CMap* map)
{
	for (int y = 0; y < map->theNumOfTiles_Height; ++y)
	{
		for (int x = 0; x < map->theNumOfTiles_Width; ++x)
		{
			modelStack.PushMatrix();
			modelStack.Translate(x * map->GetTileSize(), y * map->GetTileSize(), 0);
			modelStack.Scale(map->theTileSize, map->theTileSize, map->theTileSize);
			float opacity = 1 - (map->theMap[y][x].OpacityLevel * 0.2);
			if (map->theMap[y][x].BlockID == 0)
			{
				RenderMesh(meshList[GEO_FLOOR], false, opacity);
			}
			if (map->theMap[y][x].BlockID == 1)
			{
				RenderMesh(meshList[GEO_WALL], false, opacity);
			}
			if (map->theMap[y][x].BlockID / 100 % 10 == 2 && map->theMap[y][x].active)
			{
				RenderMesh(meshList[GEO_PLAYER1], false, opacity);
			}
			if (map->theMap[y][x].BlockID / 100 % 10 == 1 && map->theMap[y][x].active)
			{
				RenderMesh(meshList[GEO_DOOR], false, opacity);
			}
			modelStack.PopMatrix();
		}
	}
}

void SceneBase::RenderFogMap(CMap* map)
{
	for (int y = 0; y < map->theNumOfTiles_Height; ++y)
	{
		for (int x = 0; x < map->theNumOfTiles_Width; ++x)
		{
			modelStack.PushMatrix();
			modelStack.Translate(x * map->GetTileSize(), y * map->GetTileSize(), 0);
			modelStack.Scale(map->theTileSize, map->theTileSize, map->theTileSize);
			if (map->theMap[y][x].BlockID == 0)
			{
				RenderMesh(meshList[GEO_FLOOR], false);
			}
			if (map->theMap[y][x].BlockID == 1)
			{
				RenderMesh(meshList[GEO_WALL], false);
			}
			modelStack.PopMatrix();
		}
	}
}

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight, float opacity)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
		glUniform1f(m_parameters[U_TRANSPARENCY], opacity);
	}
	else
	{	
		glUniform1f(m_parameters[U_TRANSPARENCY], opacity);
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if(mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if(mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
}

void SceneBase::TrueExit()
{
    glDeleteProgram(m_programID);
    glDeleteVertexArrays(1, &m_vertexArrayID);
}
