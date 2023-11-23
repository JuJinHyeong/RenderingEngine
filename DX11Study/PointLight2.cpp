#include "PointLight2.h"
#include <string>
#include "DynamicConstant.h"
#include "SolidSphere.h"
#include "imgui/imgui.h"
#include "Camera.h"
#include "CustomMath.h"
using json = nlohmann::json;

PointLight2::PointLight2(Graphics& gfx, const std::string& name)
	:
	Light("Point Light", Light::Type::point)
{
	Dcb::RawLayout lightCBufferLayout;
	lightCBufferLayout.Add<Dcb::Float3>("position");
	lightCBufferLayout.Add<Dcb::Float3>("ambient");
	lightCBufferLayout.Add<Dcb::Float3>("diffuseColor");
	lightCBufferLayout.Add<Dcb::Float>("diffuseIntensity");
	lightCBufferLayout.Add<Dcb::Float>("attConst");
	lightCBufferLayout.Add<Dcb::Float>("attLin");
	lightCBufferLayout.Add<Dcb::Float>("attQuad");

	Dcb::Buffer buf{ std::move(lightCBufferLayout) };
	buf["position"] = DirectX::XMFLOAT3(10.0f, 9.0f, 2.5f);
	buf["ambient"] = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	buf["diffuseColor"] = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	buf["diffuseIntensity"] = 1.0f;
	buf["attConst"] = 1.0f;
	buf["attLin"] = 0.045f;
	buf["attQuad"] = 0.0075f;
	
	lightCBuffer = std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0u);

	meshPtrs.push_back(std::make_shared<SolidSphere>(gfx, 0.5f));

	pCamera = std::make_shared<Camera>(gfx, "Light", DirectX::XMFLOAT3{10.0f, 9.0f, 2.5f}, 0.0f, PI / 2, true);
}

void PointLight2::Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept {
	Dcb::Buffer buf{ lightCBuffer->GetBuffer() };
	DirectX::XMFLOAT3 pos = buf["position"];
	DirectX::XMFLOAT3 originPos = pos;
	DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), view));
	buf["position"] = pos;
	lightCBuffer->SetBuffer(buf);
	lightCBuffer->Bind(gfx);

	buf["position"] = originPos;
	lightCBuffer->SetBuffer(buf);
}

void PointLight2::SpawnControlWindow() noexcept {
	using namespace ImGui;
	if (Begin("Light")) {
		Dcb::Buffer buf{ lightCBuffer->GetBuffer() };
		bool isDirty = false;
		const auto dcheck = [&isDirty](bool dirty) {isDirty = isDirty || dirty; };

		Text("Position");
		DirectX::XMFLOAT3& pos = buf["position"];
		dcheck(SliderFloat("X", &pos.x, -60.0f, 60.0f, "%.1f"));
		dcheck(SliderFloat("Y", &pos.y, -60.0f, 60.0f, "%.1f"));
		dcheck(SliderFloat("Z", &pos.z, -60.0f, 60.0f, "%.1f"));

		if (isDirty) {
			pCamera->SetPos(pos);
		}

		Text("Intensity/Color");
		float& diffuseIntensity = buf["diffuseIntensity"];
		dcheck(SliderFloat("Intensity", &diffuseIntensity, 0.01f, 2.0f, "%.2f"));
		DirectX::XMFLOAT3& diffuseColor = buf["diffuseColor"];
		dcheck(ColorEdit3("Diffuse Color", &diffuseColor.x));
		DirectX::XMFLOAT3& ambientColor = buf["ambient"];
		dcheck(ColorEdit3("Ambient Color", &ambientColor.x));

		Text("Falloff");
		float& attConst = buf["attConst"];
		dcheck(SliderFloat("Constant", &attConst, 0.05f, 10.0f, "%.2f"));
		float& attLin = buf["attLin"];
		dcheck(SliderFloat("Linear", &attLin, 0.0001f, 1.0f, "%.4f"));
		float& attQuad = buf["attQuad"];
		dcheck(SliderFloat("Quadratic", &attQuad, 0.0000001f, 0.3f, "%.7f"));

		if (Button("Reset")) {
			Reset();
		}

		if (isDirty) {
			lightCBuffer->SetBuffer(std::move(buf));
		}
	}
	ImGui::End();
}

void PointLight2::Reset() noexcept {
	Dcb::Buffer buf{ lightCBuffer->GetBuffer() };
	buf["position"] = DirectX::XMFLOAT3(10.0f, 9.0f, 2.5f);
	buf["ambient"] = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	buf["diffuseColor"] = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	buf["diffuseIntensity"] = 1.0f;
	buf["attConst"] = 1.0f;
	buf["attLin"] = 0.045f;
	buf["attQuad"] = 0.0075f;

	lightCBuffer->SetBuffer(std::move(buf));
}

void PointLight2::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	for (const auto& pm : meshPtrs) {
		const auto& mesh = std::dynamic_pointer_cast<SolidSphere>(pm);
		// TODO: fix light mesh...
		if (mesh != nullptr) {
			Dcb::Buffer buf{ lightCBuffer->GetBuffer() };
			DirectX::XMFLOAT3 pos = buf["position"];
			mesh->SetPos(pos);
			mesh->Submit(channel);
		}
	}
}

std::shared_ptr<Camera> PointLight2::ShareCamera() const noexcept {
	return pCamera;
}
