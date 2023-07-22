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
	class Factory {
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx) {}

		std::unique_ptr<Drawable> operator()() {
			const DirectX::XMFLOAT3 mat = { cdist(rng), cdist(rng), cdist(rng) };
			
			switch (sdist(rng)) {
			case 0:
				return std::make_unique<Box>(gfx, rng, adist, ddist, odist, rdist, bdist, mat);
			case 1:
				return std::make_unique<Cylinder>(gfx, rng, adist, ddist, odist, rdist, tdist);
			case 2:
				return std::make_unique<Pyramid>(gfx, rng, adist, ddist, odist, rdist, tdist);
			case 3:
				return std::make_unique<SkinnedBox>(gfx, rng, adist, ddist, odist, rdist);
			case 4:
				return std::make_unique<AssimpTest>(gfx, rng, adist, ddist, odist, rdist, mat, 1.0f);
			default:
				assert(false && "impossible drawable option in factory");
				return nullptr;
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sdist{ 0, 4 };
		std::uniform_real_distribution<float> adist{ 0.0f, PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f, PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f, PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f, 20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f, 3.0f };
		std::uniform_real_distribution<float> cdist{ 0.0f, 1.0f};
		std::uniform_int_distribution<int> tdist{ 3, 30 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	for (auto& pd : drawables) {
		if (auto pb = dynamic_cast<Box*>(pd.get())) {
			boxes.push_back(pb);
		}
	}

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

	// imgui windows
	SpawnSimulationWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();

	for (auto& d : drawables) {
		d->Update(wnd.keyboard.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(wnd.Gfx());
	}
	light.Draw(wnd.Gfx());

	wnd.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept {

	// when minimized, begin return false.
	if (ImGui::Begin("simulation speed")) {
		ImGui::SliderFloat("speed factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
}

void App::SpawnBoxWindowManagerWindow() noexcept {

	if (ImGui::Begin("Boxes")) {
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str())) {
			for (int i = 0; i < boxes.size(); i++) {
				const bool selected = comboBoxIndex ? (*comboBoxIndex == i) : false;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected)) {
					comboBoxIndex = i;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex) {
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();

}

void App::SpawnBoxWindows() noexcept {
	for (auto it = boxControlIds.begin(); it != boxControlIds.end();) {
		if (!boxes[*it]->SpawnControlWindow(*it, wnd.Gfx())) {
			it = boxControlIds.erase(it);
		}
		else {
			it++;
		}
	}
}
