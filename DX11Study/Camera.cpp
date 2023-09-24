#include "Camera.h"
#include "imgui/imgui.h"
#include "CustomMath.h"

Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept 
	:
	name(std::move(name)),
	homePos(homePos),
	homePitch(homePitch),
	homeYaw(homeYaw),
	tethered(tethered)
{
	if (tethered) {
		pos = homePos;
	}
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept {
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// generate camera transform
	// camera looking vector in world space
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector, DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
	const auto camPosition = DirectX::XMLoadFloat3(&pos);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	// camera up direction in world always (0, 1, 0)
	return DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWidgets(Graphics& gfx) noexcept {
	ImGui::Text("Position");
	ImGui::SliderFloat("X", &pos.x, 0.2f, 80.0f, "%1.f");
	ImGui::SliderFloat("Y", &pos.y, 0.2f, 80.0f, "%1.f");
	ImGui::SliderFloat("Z", &pos.z, 0.2f, 80.0f, "%1.f");

	ImGui::Text("Orientation");
	ImGui::SliderAngle("Pitch", &pitch, -90.0f * 0.9995f, 90.0f * 0.9995f);
	ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	ImGui::SliderAngle("Roll", &roll, -90.0f, 90.0f);
	if (ImGui::Button("Reset")) {
		Reset();
	}
}

void Camera::Reset() noexcept {
	pos = { -13.5f, 6.0f, 3.5f };
	pitch = 0.0f;
	yaw = PI / 2.0f;
	roll = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept {
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept {
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		DirectX::XMMatrixScaling(moveSpeed, moveSpeed, moveSpeed)
	));
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z,
	};
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept {
	return pos;
}

const std::string& Camera::GetName() const noexcept {
	return name;
}
