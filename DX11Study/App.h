#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "PointLight.h"
#include "Mesh.h"
#include "TestPlane.h"
#include "TestCube.h"
#include "FrameCommander.h"
#include <set>
#include <vector>

class App {
public:
	App();
	int Go();
private:
	void DoFrame();
	void ShowModelDemoWindow();
	void ShowFrameRateWindow();
private:
	bool showDemoWindow = false;
	float speed_factor = 1.0f;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight light;
	FrameCommander fc;

	//Model gobber{ wnd.Gfx(), "models/gobber/GoblinX.obj", 6.0f };
	//Model nano{ wnd.Gfx(), "models/nano_textured/nanosuit.obj", 1.0f };
	//Model wall{ wnd.Gfx(), "models/brick_wall/brick_wall.obj", 2.0f };
	//TestPlane bluePlane{ wnd.Gfx(), 4.0f, {0.3f, 0.3f, 1.0f, 1.0f} };
	//TestPlane redPlane{ wnd.Gfx(), 4.0f, {1.0f, 0.3f, 0.3f, 1.0f} };
	//Model sponza{ wnd.Gfx(), "models/sponza/sponza.obj", 1/20.0f };

	TestCube cube1{ wnd.Gfx(), 4.0f };
	TestCube cube2{ wnd.Gfx(), 4.0f };
};
 