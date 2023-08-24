#include "DataMacros.h"
#include "App.h"
#include "CustomMath.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <dxtex/DirectXTex.h>
#include <vector>

App::App()
	:
	wnd(1980, 1080, "First App"),
	light(wnd.Gfx()) 
{
	// TODO command line scripts
	bluePlane.SetPos(cam.GetPos());
	redPlane.SetPos(cam.GetPos());
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 720.0f / 1280.0f, 0.5f, 400.0f));
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

	//nano.Draw(wnd.Gfx());
	//gobber.Draw(wnd.Gfx());
	//wall.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw(wnd.Gfx());
	redPlane.Draw(wnd.Gfx());

	while (const auto e = wnd.keyboard.ReadKey()) {
		if (!e->IsPress()) {
			continue;
		}
		switch (e->GetCode()) {
		case VK_ESCAPE:
			if (wnd.CursorEnabled()) {
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else {
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = !showDemoWindow;
			break;
		default:
			break;
		}
	}

	if (!wnd.CursorEnabled()) {
		if (wnd.keyboard.KeyIsPressed('W')) {
			cam.Translate({ 0.0f, 0.0f, dt });
		}
		if (wnd.keyboard.KeyIsPressed('A')) {
			cam.Translate({ -dt, 0.0f, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('S')) {
			cam.Translate({ 0.0f, 0.0f, -dt });
		}
		if (wnd.keyboard.KeyIsPressed('D')) {
			cam.Translate({ dt, 0.0f, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('R')) {
			cam.Translate({ 0.0f, dt, 0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('F')) {
			cam.Translate({ 0.0f, -dt, 0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			cam.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}

	if (showDemoWindow) {
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		ShowModelDemoWindow();
	}

	wnd.Gfx().EndFrame();
}

void App::ShowModelDemoWindow() {
	//nano.ShowWindow("Nano");
	//gobber.ShowWindow("Goblin");
	//wall.ShowWindow("Wall");
	bluePlane.SpawnControlWindow(wnd.Gfx(), "blue plane");
	redPlane.SpawnControlWindow(wnd.Gfx(), "red plane");
	sponza.ShowWindow("Sponza");
}

void App::ShowFrameRateWindow() {
	if (ImGui::Begin("Frame Rate")) {
		ImGui::Text("%d fps", static_cast<int>(1.0f / timer.Mark()));
	}
	ImGui::End();
}
