#include "DataMacros.h"
#include "App.h"
#include <vector>
#include "Box.h"
#include "Pyramid.h"
#include "Melon.h"
#include "CustomMath.h"
#include "Sheet.h"
#include "Cylinder.h"
#include "SkinnedBox.h"
#include "AssimpTest.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GDIPlusManager gdipm;

App::App() 
	:
	wnd(WINDOW_WIDTH, WINDOW_HEIGHT, "First App"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}


int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessage()) {
			return *ecode;
		}
		DoFrame();
	}

	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}
	if (gResult == -1) {
		throw WND_LAST_EXCEPT();
	}

	return (int)msg.wParam;
}

void App::DoFrame() {
	const auto dt = timer.Mark() * speed_factor;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	nano.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelDemoWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowModelDemoWindow() {
	nano.ShowWindow("nanosuit.obj");
}
