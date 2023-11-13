#pragma once
#include <DirectXMath.h>

class Object{
public:
	Object() = default;
	virtual const DirectX::XMFLOAT4X4& GetTransform() const noexcept = 0;
};