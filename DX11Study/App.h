#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "Drawable.h"
#include "PointLight.h"
#include <set>
#include <vector>

class App {
public:
	App();
	int Go();
private:
	void DoFrame();
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
private:
	float speed_factor = 1.0f;
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	std::vector<std::unique_ptr<Drawable>> drawables;
	std::vector<class Box*> boxes;
	static constexpr size_t nDrawables = 180;
	Camera cam;
	PointLight light;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};
