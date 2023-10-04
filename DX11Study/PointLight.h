#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

namespace Rgph {
	class RenderGraph;
}
class Camera;

class PointLight {
public:
	PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit(size_t channel) const noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg);
	std::shared_ptr<Camera> ShareCamera() const noexcept;
private:
	struct PointLightCBuf {
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};

private:
	PointLightCBuf home;
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;
	std::shared_ptr<Camera> pCamera;
};