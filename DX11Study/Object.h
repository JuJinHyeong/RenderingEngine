#pragma once
#include <DirectXMath.h>

namespace Rgph {
	class RenderGraph;
}

class Object{
public:
	Object() = default;
	virtual const DirectX::XMFLOAT4X4& GetTransform() const noexcept = 0;
	virtual void Submit(size_t channel) noexcept(!IS_DEBUG) = 0;
	virtual void LinkTechniques(Rgph::RenderGraph& rg) = 0;
};