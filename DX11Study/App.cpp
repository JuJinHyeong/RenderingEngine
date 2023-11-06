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

constexpr static int width = 1280;
constexpr static int height = 720;

App::App()
	: wnd(width, height, "First App"),
	  light(wnd.Gfx(), {10.0f, 4.0f, 0.0f})
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "First", DirectX::XMFLOAT3{0.0f, 0.0f, -60.0f}, 0.0f, 0.0f, false));
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "Second", DirectX::XMFLOAT3{-13.5f, 28.0f, -3.5f}, 0.0f, PI / 2.0f, false));
	cameras.AddCamera(light.ShareCamera());

	// cube1.SetPos({ 23.0f,5.0f,6.0f });
	// cube2.SetPos({ 10.0f,5.0f,14.0f });

	//nano.SetRootTransform(DirectX::XMMatrixRotationY(PI / 2) * DirectX::XMMatrixTranslation(25.0f, 0.0f, 0.0f));

	light.LinkTechniques(rg);
	// cube1.LinkTechniques(rg);
	// cube2.LinkTechniques(rg);
	// gobber.LinkTechniques(rg);
	//nano.LinkTechniques(rg);
	//sponza.LinkTechniques(rg);
	testModel.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());
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
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());

	light.Submit(channel::main);
	// cube1.Submit(channel::main);
	// cube2.Submit(channel::main);
	//sponza.Submit(channel::main);
	//nano.Submit(channel::main);
	// gobber.Submit(channel::main);
	// cameras.Submit(channel::main);
	testModel.Submit(channel::main);

	//sponza.Submit(channel::shadow);
	// cube1.Submit(channel::shadow);
	// cube2.Submit(channel::shadow);
	// gobber.Submit(channel::shadow);
	//nano.Submit(channel::shadow);

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
		cameras.SpawnWindow(wnd.Gfx());
		light.SpawnControlWindow();
		//sponzaProbe.SpawnWindow(sponza);
		//nanoProbe.SpawnWindow(nano);
		// gobberProbe.SpawnWindow(gobber);
		// cube1.SpawnControlWindow(wnd.Gfx(), "cube1");
		// cube2.SpawnControlWindow(wnd.Gfx(), "cube2");
		nanoProbe.SpawnWindow(testModel);
		rg.RenderWindows(wnd.Gfx());

		ImGui::Begin("view proj transform");
		DirectX::XMFLOAT4X4 view;
		DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixTranspose(wnd.Gfx().GetCamera()));
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMStoreFloat4x4(&proj, DirectX::XMMatrixTranspose(wnd.Gfx().GetProjection()));
		ImGui::Text("View");
		ImGui::Text("%f %f %f %f", view._11, view._12, view._13, view._14);
		ImGui::Text("%f %f %f %f", view._21, view._22, view._23, view._24);
		ImGui::Text("%f %f %f %f", view._31, view._32, view._33, view._34);
		ImGui::Text("%f %f %f %f", view._41, view._42, view._43, view._44);
		ImGui::Text("Proj");
		ImGui::Text("%f %f %f %f", proj._11, proj._12, proj._13, proj._14);
		ImGui::Text("%f %f %f %f", proj._21, proj._22, proj._23, proj._24);
		ImGui::Text("%f %f %f %f", proj._31, proj._32, proj._33, proj._34);
		ImGui::Text("%f %f %f %f", proj._41, proj._42, proj._43, proj._44);

		ImGui::End();
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
			cameras->Translate({0.0f, 0.0f, dt});
		}
		if (wnd.keyboard.KeyIsPressed('A'))
		{
			cameras->Translate({-dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('S'))
		{
			cameras->Translate({0.0f, 0.0f, -dt});
		}
		if (wnd.keyboard.KeyIsPressed('D'))
		{
			cameras->Translate({dt, 0.0f, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('R'))
		{
			cameras->Translate({0.0f, dt, 0.0f});
		}
		if (wnd.keyboard.KeyIsPressed('F'))
		{
			cameras->Translate({0.0f, -dt, 0.0f});
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cameras->Rotate((float)delta->x, (float)delta->y);
		}
	}
}
