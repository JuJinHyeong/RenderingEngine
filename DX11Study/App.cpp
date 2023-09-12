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

App::App()
	:
	wnd(1980, 1080, "First App"),
	light(wnd.Gfx()) 
{
	// TODO command line scripts
	//bluePlane.SetPos(cam.GetPos());
	//redPlane.SetPos(cam.GetPos());

	{
		std::string path = "models\\brick_wall\\brick_wall.obj";
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(path,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		Material mat{ wnd.Gfx(),*pScene->mMaterials[1], path };
		pLoaded = std::make_unique<Mesh>(wnd.Gfx(), mat, *pScene->mMeshes[0]);
	}

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
	//light.Draw(wnd.Gfx());
	//sponza.Draw(wnd.Gfx());
	//bluePlane.Draw(wnd.Gfx());
	//redPlane.Draw(wnd.Gfx());
	//cube1.Draw(wnd.Gfx());
	//cube2.Draw(wnd.Gfx());
	//cube1.DrawOutLine(wnd.Gfx());
	//cube2.DrawOutLine(wnd.Gfx());

	light.Submit(fc);
	//cube1.Submit(fc);
	//cube2.Submit(fc);
	//wall.Submit(fc);
	//gobber.Submit(fc);
	pLoaded->Submit(fc, DirectX::XMMatrixIdentity());
	fc.Execute(wnd.Gfx());

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

	class Probe : public TechniqueProbe {
	public:
		void OnSetTechnique() override {
			using namespace std::string_literals;
			ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, pTech->GetName().c_str());
			bool active = pTech->IsActive();
			ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
			pTech->SetActiveState(active);
		}
		bool OnVisitBuffer(Dcb::Buffer& buf) override {
			float dirty = false;
			const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
			auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
			(const char* label) mutable {
				tagScratch = label + tagString;
				return tagScratch.c_str();
			};

			if (auto v = buf["scale"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f"));
			}
			if (auto v = buf["materialColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularColor"]; v.Exists()) {
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v))));
			}
			if (auto v = buf["specularGloss"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
			}
			if (auto v = buf["specularWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if (auto v = buf["useNormalMap"]; v.Exists()) {
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if (auto v = buf["normalMapWeight"]; v.Exists()) {
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	} probe;

	pLoaded->Accept(probe);
	
	if (showDemoWindow) {
		cam.SpawnControlWindow();
		light.SpawnControlWindow();
		ShowModelDemoWindow();
	}

	wnd.Gfx().EndFrame();
	fc.Reset();
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
