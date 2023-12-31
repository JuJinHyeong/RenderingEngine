#include "PointLight.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "CustomMath.h"

PointLight::PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos, float radius) 
	:
	mesh(gfx, radius),
	cbuf(gfx)
{
	home = {
		pos,
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
	Reset();
	pCamera = std::make_shared<Camera>(gfx, "Light", cbData.pos, 0.0f, PI / 2, true);
}

void PointLight::SpawnControlWindow() noexcept {
	using namespace ImGui;
	if (Begin("Light")) {
		bool dirtyPos = false;
		const auto dcheck = [&dirtyPos](bool dirty) {dirtyPos = dirtyPos || dirty; };

		Text("Position");
		dcheck(SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f"));
		dcheck(SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f"));
		dcheck(SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f"));

		if (dirtyPos) {
			pCamera->SetPos(cbData.pos);
		}

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

void PointLight::Submit(size_t channel) noexcept(!IS_DEBUG) {
	mesh.SetPos(cbData.pos);
	mesh.Submit(channel);
}

void PointLight::Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept {
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}

void PointLight::LinkTechniques(Rgph::RenderGraph& rg) {
	mesh.LinkTechniques(rg);
}

std::shared_ptr<Camera> PointLight::ShareCamera() const noexcept {
	return pCamera;
}
