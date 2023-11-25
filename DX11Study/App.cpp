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
#include "Material.h"
#include "TestModelProbe.h"
#include "ExtendedXMMath.h"
#include "Camera.h"
#include "Channels.h"
#include <iostream>
#include "SceneModifier.h"
#include "PointLight2.h"
#include "Geometry.h"
#include "CubeMesh.h"
#include "EmptySceneObject2.h"

#include "Testing.h"

constexpr static int width = 1980;
constexpr static int height = 1020;

App::App()
	:
	wnd(width, height, "First App")
{
	// initialize scene

	pPointLight = std::make_shared<PointLight2>(wnd.Gfx(), "Point Light");
	rg.BindShadowCamera(*pPointLight->ShareCamera());
	

	auto pCameras = std::make_unique<CameraContainer>();
	pCameras->AddCamera(std::make_unique<Camera>(wnd.Gfx(), "First", DirectX::XMFLOAT3{ 0.0f, 0.0f, -60.0f }, 0.0f, 0.0f, false));
	pCameras->AddCamera(std::make_unique<Camera>(wnd.Gfx(), "Second", DirectX::XMFLOAT3{ -13.5f, 28.0f, -3.5f }, 0.0f, PI / 2.0f, false));
	pCameras->AddCamera(pPointLight->ShareCamera());

	scene2.AddSceneObject(wnd.Gfx(), "models/nano_textured/nanosuit.obj");
	scene2.AddSceneObject(wnd.Gfx(), "models/gobber/GoblinX.obj", 3.0f);
	auto pCubeParent = std::make_shared<EmptySceneObject2>("Cube Parent");
	pCubeParent->AddChild(std::make_shared<Geometry<CubeMesh>>(wnd.Gfx(), "cube1"));
	pCubeParent->AddChild(std::make_shared<Geometry<CubeMesh>>(wnd.Gfx(), "cube2"));
	scene2.AddSceneObject(std::move(pCubeParent));

	scene2.AddSceneObject(wnd.Gfx(), "models/sponza/sponza.obj", 1 / 20.0f);
	scene2.AddSceneObject(pPointLight);
	scene2.SetCameraContainer(std::move(pCameras));

	scene2.LinkTechnique(rg);
}

int App::Go()
{
	while (true)
	{
		if (const auto ecode = Window::ProcessMessage())
		{
			return *ecode;
		}
		const auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		DoFrame(dt);
	}
}

void App::DoFrame(float dt)
{
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	rg.BindMainCamera(scene2.GetActiveCamera());

	scene2.Bind(wnd.Gfx());
	scene2.Submit(channel::main);
	scene2.Submit(channel::shadow);

	rg.Execute(wnd.Gfx());

	if (savingDepth)
	{
		rg.DumpShadowMap(wnd.Gfx(), "shadow.png");
		savingDepth = false;
	}

	static MP sponzaProbe("sponza");
	static MP nanoProbe("nano");
	static MP gobberProbe("goblin");

	if (showDemoWindow)
	{
		ShowWindow();
		scene2.ShowWindow();
		rg.RenderWindows(wnd.Gfx());
		pPointLight->SpawnControlWindow();
	}

	wnd.Gfx().EndFrame();
	rg.Reset();
}

void App::HandleInput(float dt)
{
	while (const auto e = wnd.keyboard.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = !showDemoWindow;
			break;
		case VK_RETURN:
			savingDepth = true;
			break;
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.keyboard.KeyIsPressed('W'))
		{
			scene2.GetActiveCamera().Translate({0.0f, 0.0f, dt});
		}
		if (wnd.keyboard.KeyIsPressed('A'))
		{
			scene2.GetActiveCamera().Translate({-dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('S'))
		{
			scene2.GetActiveCamera().Translate({0.0f, 0.0f, -dt});
		}
		if (wnd.keyboard.KeyIsPressed('D'))
		{
			scene2.GetActiveCamera().Translate({dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('R'))
		{
			scene2.GetActiveCamera().Translate({0.0f, dt, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('F'))
		{
			scene2.GetActiveCamera().Translate({0.0f, -dt, 0.0f});
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			scene2.GetActiveCamera().Rotate((float)delta->x, (float)delta->y);
		}
	}
}

char buffer[256];
void App::ShowWindow()
{
	// gpt
	ImGui::Begin("GPT");
	ImGui::Columns(2, nullptr, true);
	ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "Makeable Object List");
	std::vector<std::string> makeableNames;
	auto& makeables = scene2.GetMakeableObjects();
	for (auto makeable : makeables) {
		ImGui::Text(makeable.first.c_str());
		makeableNames.push_back(makeable.first);
	}
	if (ImGui::Button("Dump To Json")) {
		std::cout << scene2.ToJson().dump(4) << std::endl;
	}
	ImGui::NextColumn();
	ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, "GPT Message");
	ImGui::InputText(" ", buffer, IM_ARRAYSIZE(buffer));
	if (ImGui::Button("send")) {
		SceneModifier::SendToGpt(wnd.Gfx(), scene2, makeableNames, buffer);
		scene2.LinkTechnique(rg);
	}
	ImGui::End();
}
