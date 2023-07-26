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
	wnd(1280, 720, "First App"),
	light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 720.0f / 1280.0f, 0.5f, 40.0f));
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

	while (const auto e = wnd.keyboard.ReadKey()) {
		if (e->IsPress() && e->GetCode() == VK_MENU) {
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
		}
		else if (e->IsRelease() && e->GetCode() == VK_MENU) {
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
		}
	}

	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelDemoWindow();
	ShowRawInputWindow();

	wnd.Gfx().EndFrame();
}

void App::ShowModelDemoWindow() {
	nano.ShowWindow("nanosuit.obj");
}

void App::ShowRawInputWindow() {
	while (const auto d = wnd.mouse.ReadRawDelta()) {
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input")) {
		ImGui::Text("Mouse Delta (%d, %d)", x, y);
		ImGui::Text("Cursor: %s", wnd.CursorEnabled() ? "Enabled" : "Disabled");
	}
	ImGui::End();
}
