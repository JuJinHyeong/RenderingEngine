#pragma once
#include "Graphics.h"
#include "Projection.h"
#include "CameraIndicator.h"

namespace Rgph {
	class RenderGraph;
}

class Camera {
public:
	Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered = false) noexcept;
	~Camera() = default;
	void BindToGraphics(Graphics& gfx) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset(Graphics& gfx) noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel) const;

private:
	bool tethered;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 homePos;
	float pitch;
	float homePitch;
	float yaw;
	float homeYaw;
	float roll;
	static constexpr float moveSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	bool enableCameraIndicator = true;
	bool enableFrustumIndicator = true;
	std::string name;
	Projection proj;
	CameraIndicator indicator;
};
