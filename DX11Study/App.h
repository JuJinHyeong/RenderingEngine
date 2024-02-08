#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "TestPlane.h"
#include "TestCube.h"
#include "BlurOutlineRenderGraph.h"
#include "CameraContainer.h"
#include "PointLight2.h"
#include <set>
#include <vector>

#include "Scene2.h"

class App {
public:
	App();
	int Go();
private:
	void DoFrame(float dt);
	void HandleInput(float dt);
	void ShowWindow();
private:
	bool showDemoWindow = false;
	float speed_factor = 1.0f;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	//CameraContainer cameras;
	//PointLight light;

	// TODO: add model group
	Scene2 scene2{ "second scene" };
	std::shared_ptr<PointLight2> pPointLight;
	//Model gobber{ wnd.Gfx(), "models/gobber/GoblinX.obj", 3.0f };
	//Model nano{ wnd.Gfx(), "models/nano_textured/nanosuit.obj", 1.0f };
	//Model wall{ wnd.Gfx(), "models/brick_wall/brick_wall.obj", 2.0f };
	//TestPlane bluePlane{ wnd.Gfx(), 4.0f, {0.3f, 0.3f, 1.0f, 1.0f} };
	//TestPlane redPlane{ wnd.Gfx(), 4.0f, {1.0f, 0.3f, 0.3f, 1.0f} };
	//Model sponza{ wnd.Gfx(), "models/sponza/sponza.obj", 1/20.0f };
	//Model testModel{wnd.Gfx(), "models/boblampclean/boblampclean.md5mesh", 1.0f};
	//Model testModel{ wnd.Gfx(), "models/man/model.dae", 1.0f };

	Rgph::BlurOutlineRenderGraph rg{wnd.Gfx()};

	//TestCube cube1{ wnd.Gfx(), 4.0f };
	//TestCube cube2{ wnd.Gfx(), 4.0f };

	const std::vector<std::string> makeables{ "cube", "cone", "plane", "prism", "sphere", "nano", "goblin" };
	bool savingDepth = false;
};
 