#pragma once
#include <DirectXMath.h>

namespace Rgph {
	class RenderGraph;
}

class Object {
public:
	Object() = default;
	virtual void Submit(size_t channel) noexcept(!IS_DEBUG) = 0;
	virtual void LinkTechniques(Rgph::RenderGraph& rg) = 0;
	DirectX::XMMATRIX& GetTransform() noexcept;
	void Transform(DirectX::XMVECTOR translation, DirectX::XMVECTOR quaternion, DirectX::XMVECTOR scale) noexcept;
protected:
	DirectX::XMMATRIX transform;
};