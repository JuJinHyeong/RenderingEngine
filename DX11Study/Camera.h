#pragma once
#include "Graphics.h"

class Camera {
public:
	Camera() = default;
	~Camera() = default;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

private:
	// position
	float r = 20.0f;
	float theta = 0.0f;
	float phi = 0.0f;

	// rotation
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};
