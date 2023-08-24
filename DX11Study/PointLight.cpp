#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& gfx, float radius) 
	:
	mesh(gfx, radius),
	cbuf(gfx)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept {
	using namespace ImGui;
	if (Begin("Light")) {
		Text("Position");
		SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		Text("Intensity/Color");
		SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f");
		ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ColorEdit3("Ambient Color", &cbData.ambient.x);

		Text("Falloff");
		SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f");
		SliderFloat("Linear", &cbData.attLin, 0.0001f, 1.0f, "%.4f");
		SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 0.3f, "%.7f");

		if (Button("Reset")) {
			Reset();
		}
	}
	End();
}

void PointLight::Reset() noexcept {
	cbData = {
		{ 10.0f, 9.0f, 2.5f},
		{ 0.05f, 0.05f, 0.05f },
		{ 1.0f, 1.0f, 1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG) {
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept {
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}