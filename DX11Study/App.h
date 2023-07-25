#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "PointLight.h"
#include "Model.h"
#include <set>
#include <vector>

class App {
public:
	App();
	int Go();
private:
	void DoFrame();
	void ShowModelWindow();
private:
	float speed_factor = 1.0f;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(),"models/nanosuit.obj" };

	struct {
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;
};
