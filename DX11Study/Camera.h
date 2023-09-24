#pragma once
#include "Graphics.h"
#include "Projection.h"

class Camera {
public:
	Camera(Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept;
	~Camera() = default;
	void BindToGraphics(Graphics& gfx) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	const std::string& GetName() const noexcept;

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
	std::string name;
	Projection proj;
};
