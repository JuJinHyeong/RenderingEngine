#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "PointLight.h"
#include "Mesh.h"
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
	Model nano{ wnd.Gfx(),"models/nano_textured/nanosuit.obj" };
	Model nano2{ wnd.Gfx(),"models/nano_textured/nanosuit.obj" };
};
