#pragma once
#include "Drawable.h"

class BaseMesh : public Drawable {
public:
	BaseMesh(Graphics& gfx);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
private:
	mutable DirectX::XMFLOAT4X4 transform;
};