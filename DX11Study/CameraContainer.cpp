#include "CameraContainer.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "Graphics.h"

void CameraContainer::SpawnWindow(Graphics& gfx) {
	if (ImGui::Begin("Cameras")) {
		if (ImGui::BeginCombo("Active Camera", cameras[selected]->GetName().c_str())) {
			for (size_t i = 0; i < cameras.size(); i++) {
				const bool isSelected = (selected == i);
				if (ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected)) {
					selected = i;
				}
			}
			ImGui::EndCombo();
		}
		GetCurCamera().SpawnControlWidgets(gfx);
	}
	ImGui::End();
}

void CameraContainer::Bind(Graphics& gfx) {
	gfx.SetCamera(GetCurCamera().GetMatrix());
}

void CameraContainer::AddCamera(std::unique_ptr<Camera> pCam) {
	cameras.push_back(std::move(pCam));
}

Camera& CameraContainer::GetCurCamera() {
	return *cameras[selected];
}

CameraContainer::~CameraContainer() {}

void CameraContainer::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& pCam : cameras) {
		pCam->LinkTechniques(rg);
	}
}

void CameraContainer::Submit() const {
	for (size_t i = 0; i < cameras.size(); i++) {
		if (i != selected) {
			cameras[i]->Submit();
		}
	}
}
