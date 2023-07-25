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
	//class Factory {
	//public:
	//	Factory(Graphics& gfx)
	//		:
	//		gfx(gfx) {}

	//	std::unique_ptr<Drawable> operator()() {
	//		const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
	//		
	//		switch (sdist(rng)) {
	//		case 0:
	//			return std::make_unique<Box>(gfx, rng, adist, ddist, odist, rdist, bdist, mat);
	//		case 1:
	//			return std::make_unique<Cylinder>(gfx, rng, adist, ddist, odist, rdist, tdist);
	//		case 2:
	//			return std::make_unique<Pyramid>(gfx, rng, adist, ddist, odist, rdist, tdist);
	//		case 3:
	//			return std::make_unique<SkinnedBox>(gfx, rng, adist, ddist, odist, rdist);
	//		case 4:
	//			return std::make_unique<AssimpTest>(gfx, rng, adist, ddist, odist, rdist, mat, 1.0f);
	//		default:
	//			assert(false && "impossible drawable option in factory");
	//			return nullptr;
	//		}
	//	}
	//private:
	//	Graphics& gfx;
	//	std::mt19937 rng{ std::random_device{}() };
	//	std::uniform_int_distribution<int> sdist{ 0, 4 };
	//	std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
	//	std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
	//	std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
	//	std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
	//	std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
	//	std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f};
	//	std::uniform_int_distribution<int> tdist{ 3, 30 };
	//};

	//Factory f(wnd.Gfx());

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

	const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) * DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	nano.Draw(wnd.Gfx(), transform);
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();
	light.Draw(wnd.Gfx());

	wnd.Gfx().EndFrame();
}

void App::ShowModelWindow() {
	if (ImGui::Begin("Model")) {
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &pos.roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pos.pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &pos.yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &pos.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &pos.z, -20.0f, 20.0f);
	}
	ImGui::End();
}
