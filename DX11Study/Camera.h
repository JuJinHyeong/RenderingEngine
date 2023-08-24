#pragma once
#include "Graphics.h"

class Camera {
public:
	Camera() noexcept;
	~Camera() = default;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;

private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	float roll;
	static constexpr float moveSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
};
