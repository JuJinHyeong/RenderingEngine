#include "Camera.h"
#include "imgui/imgui.h"
#include "CustomMath.h"

Camera::Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept
	:
	name(std::move(name)),
	homePos(homePos),
	homePitch(homePitch),
	homeYaw(homeYaw),
	tethered(tethered),
	proj(gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f),
	indicator(gfx)
{
	if (tethered) {
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	Reset(gfx);
}

void Camera::BindToGraphics(Graphics& gfx) const {
	gfx.SetCamera(GetMatrix());
	gfx.SetProjection(proj.GetMatrix());
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
	bool rotDirty = false;
	bool posDirty = false;
	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };
	if (!tethered) {
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
	}
	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
	dcheck(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f), rotDirty);
	proj.RenderWidgets(gfx);
	ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
	ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);
	if (ImGui::Button("Reset")) {
		Reset(gfx);
	}

	if (rotDirty) {
		const DirectX::XMFLOAT3 angles = { pitch,yaw,0.0f };
		indicator.SetRotation(angles);
		proj.SetRotation(angles);
	}
	if (posDirty) {
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
}

void Camera::Reset(Graphics& gfx) noexcept {
	if (!tethered) {
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	pitch = homePitch;
	yaw = homeYaw;
	roll = 0.0f;

	const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);
	proj.Reset(gfx);
}

void Camera::Rotate(float dx, float dy) noexcept {
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept {
	if (!tethered) {
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
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
}

void Camera::SetPos(const DirectX::XMFLOAT3& pos) noexcept {
	this->pos = pos;
	indicator.SetPos(pos);
	proj.SetPos(pos);
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept {
	return pos;
}

const std::string& Camera::GetName() const noexcept {
	return name;
}

void Camera::LinkTechniques(Rgph::RenderGraph& rg) {
	indicator.LinkTechniques(rg);
	proj.LinkTechniques(rg);
}

void Camera::Submit(size_t channel) const {
	if (enableCameraIndicator) {
		indicator.Submit(channel);
	}
	if (enableFrustumIndicator) {
		proj.Submit(channel);
	}
}
