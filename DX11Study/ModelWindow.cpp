#include "ModelWindow.h"
#include "imgui/imgui.h"

void ModelWindow::Show(const char* windowName, const Node& root) noexcept(!IS_DEBUG) {
	windowName = windowName ? windowName : "Model";
	if (ImGui::Begin(windowName)) {
		ImGui::Columns(2, nullptr, true);
		root.ShowTree(pSelectedNode);

		ImGui::NextColumn();
		if (pSelectedNode != nullptr) {
			ImGui::Text("Id: %d", pSelectedNode->GetId());
			auto& transform = transforms[pSelectedNode->GetId()];
			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);

			ImGui::Text("Position");
			ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
		}
	}
	ImGui::End();
}

DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept(!IS_DEBUG) {
	assert(pSelectedNode != nullptr);
	const auto& transform = transforms.at(pSelectedNode->GetId());
	return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw)
		* DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
}

Node* ModelWindow::GetSelectedNode() const noexcept(!IS_DEBUG) {
	return pSelectedNode;
}