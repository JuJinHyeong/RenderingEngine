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
#include "Testing.h"
#include "Material.h"
#include "TestModelProbe.h"
#include "ExtendedXMMath.h"

int width = 1280;
int height = 720;

App::App()
	:
	wnd(width, height, "First App"),
	light(wnd.Gfx()) 
{
	cube1.SetPos({ 4.0f, 0.0f, 0.0f });
	cube2.SetPos({ 0.0f, 4.0f, 0.0f });

	cube1.LinkTechniques(rg);
	cube2.LinkTechniques(rg);
	sponza.LinkTechniques(rg);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, static_cast<float>(height) / static_cast<float>(width), 0.5f, 400.0f));
}


int App::Go() {
	while (true) {
		if (const auto ecode = Window::ProcessMessage()) {
			return *ecode;
		}
		const auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		DoFrame(dt);
	}
}

void App::DoFrame(float dt) {
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());
	light.Bind(wnd.Gfx(), cam.GetMatrix());

	cube1.Submit();
	cube2.Submit();
	sponza.Submit();

	rg.Execute(wnd.Gfx());

	static MP modelProbe;

	if (showDemoWindow) {
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		ShowModelDemoWindow();
		modelProbe.SpawnWindow(sponza);
		cube1.SpawnControlWindow(wnd.Gfx(), "cube1");
		cube2.SpawnControlWindow(wnd.Gfx(), "cube2");
	}

	wnd.Gfx().EndFrame();
	rg.Reset();
}

void App::HandleInput(float dt) {
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
			showDemoWindow = true;
			break;
		}
	}

	if (!wnd.CursorEnabled()) {
		if (wnd.keyboard.KeyIsPressed('W')) {
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.keyboard.KeyIsPressed('A')) {
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('S')) {
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.keyboard.KeyIsPressed('D')) {
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('R')) {
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.keyboard.KeyIsPressed('F')) {
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta()) {
		if (!wnd.CursorEnabled()) {
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}
}

void App::ShowModelDemoWindow() {
	//nano.ShowWindow("Nano");
	//gobber.ShowWindow("Goblin");
	//wall.ShowWindow("Wall");
	//bluePlane.SpawnControlWindow(wnd.Gfx(), "blue plane");
	//redPlane.SpawnControlWindow(wnd.Gfx(), "red plane");
	//sponza.ShowWindow("Sponza");
	//cube1.SpawnControlWindow(wnd.Gfx(), "cube1");
	//cube2.SpawnControlWindow(wnd.Gfx(), "cube2");
}

void App::ShowFrameRateWindow() {
	if (ImGui::Begin("Frame Rate")) {
		ImGui::Text("%d fps", static_cast<int>(1.0f / timer.Mark()));
	}
	ImGui::End();
}
