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
	void Submit(size_t channel, const DirectX::FXMMATRIX& accumulatedTransform = DirectX::XMMatrixIdentity()) const noexcept(!IS_DEBUG);
	void Accept(TechniqueProbe& probe) noexcept override;
	std::shared_ptr<Camera> ShareCamera() const noexcept;
	void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) override;
private:
	std::shared_ptr<Camera> pCamera;
};