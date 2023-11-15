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

#include "Testing.h"

constexpr static int width = 1280;
constexpr static int height = 720;

App::App()
	:
	wnd(width, height, "First App")
{
	// initialize scene
	auto pPointLight = std::make_unique<PointLight>(wnd.Gfx(), DirectX::XMFLOAT3{ 10.0f, 4.0f, 0.0f });
	auto pCameras = std::make_unique<CameraContainer>();
	auto pGobber = std::make_unique<Model>(wnd.Gfx(), "models/gobber/GoblinX.obj", 3.0f);
	auto pNano = std::make_unique<Model>(wnd.Gfx(), "models/nano_textured/nanosuit.obj", 1.0f);
	auto pSponza = std::make_unique<Model>(wnd.Gfx(), "models/sponza/sponza.obj", 1 / 20.0f);

	pCameras->AddCamera(std::make_unique<Camera>(wnd.Gfx(), "First", DirectX::XMFLOAT3{ 0.0f, 0.0f, -60.0f }, 0.0f, 0.0f, false));
	pCameras->AddCamera(std::make_unique<Camera>(wnd.Gfx(), "Second", DirectX::XMFLOAT3{ -13.5f, 28.0f, -3.5f }, 0.0f, PI / 2.0f, false));
	pCameras->AddCamera(pPointLight->ShareCamera());

	rg.BindShadowCamera(*pPointLight->ShareCamera());

	scene.AddObject(std::make_shared<SceneObject>("Point Light", std::move(pPointLight), SceneObject::Type::light));
	scene.AddObject(std::make_shared<SceneObject>("Cameras", std::move(pCameras), SceneObject::Type::camera));
	scene.AddObject(std::make_shared<SceneObject>("Goblin", std::move(pGobber), SceneObject::Type::object));
	scene.AddObject(std::make_shared<SceneObject>("Nano", std::move(pNano), SceneObject::Type::object));
	scene.AddObject(std::make_shared<SceneObject>("Sponza", std::move(pSponza), SceneObject::Type::object));

	for (const auto& obj : scene.GetObjects()) {
		obj->LinkTechniques(rg);
	}
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
	rg.BindMainCamera(scene.GetActiveCamera());

	scene.Bind(wnd.Gfx());

	scene.Submit(channel::main);
	scene.Submit(channel::shadow);

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
		scene.ShowWindow();
		rg.RenderWindows(wnd.Gfx());
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
			scene.GetActiveCamera().Translate({0.0f, 0.0f, dt});
		}
		if (wnd.keyboard.KeyIsPressed('A'))
		{
			scene.GetActiveCamera().Translate({-dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('S'))
		{
			scene.GetActiveCamera().Translate({0.0f, 0.0f, -dt});
		}
		if (wnd.keyboard.KeyIsPressed('D'))
		{
			scene.GetActiveCamera().Translate({dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('R'))
		{
			scene.GetActiveCamera().Translate({0.0f, dt, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('F'))
		{
			scene.GetActiveCamera().Translate({0.0f, -dt, 0.0f});
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			scene.GetActiveCamera().Rotate((float)delta->x, (float)delta->y);
		}
	}
}
