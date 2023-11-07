#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "TestCube.h"
#include "Model.h"
#include "BlurOutlineRenderGraph.h"
#include "CameraContainer.h"
#include <set>
#include <vector>

class App {
public:
	App();
	int Go();
private:
	void DoFrame(float dt);
	void HandleInput(float dt);
private:
	bool showDemoWindow = false;
	float speed_factor = 1.0f;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	CameraContainer cameras;
	PointLight light;

	// TODO: add model group
	//Model gobber{ wnd.Gfx(), "models/gobber/GoblinX.obj", 3.0f };
	//Model testModel{ wnd.Gfx(), "models/nano_textured/nanosuit.obj", 1.0f };
	//Model wall{ wnd.Gfx(), "models/brick_wall/brick_wall.obj", 2.0f };
	//TestPlane bluePlane{ wnd.Gfx(), 4.0f, {0.3f, 0.3f, 1.0f, 1.0f} };
	//TestPlane redPlane{ wnd.Gfx(), 4.0f, {1.0f, 0.3f, 0.3f, 1.0f} };
	//Model testModel{ wnd.Gfx(), "models/sponza/sponza.obj", 1/20.0f };
	//Model testModel{wnd.Gfx(), "models/boblampclean/boblampclean.md5mesh", 1.0f};
	Model testModel{ wnd.Gfx(), "models/man/model.dae", 1.0f };

	Rgph::BlurOutlineRenderGraph rg{wnd.Gfx()};

	//TestCube cube1{ wnd.Gfx(), 4.0f };
	//TestCube cube2{ wnd.Gfx(), 4.0f };

	bool savingDepth = false;
};
 