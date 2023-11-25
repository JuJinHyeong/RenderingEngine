#pragma once
#include "Light.h"
#include <string>
#include "Camera.h"

class PointLight2 : public Light {
public:
	PointLight2(Graphics& gfx, const std::string& name);
	void Bind(Graphics& gfx, DirectX::XMMATRIX view) const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Submit(size_t channel) const noexcept(!IS_DEBUG);
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) override;
private:
	struct PointLightCBuf {
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	} buf;
	std::shared_ptr<Camera> pCamera;
};