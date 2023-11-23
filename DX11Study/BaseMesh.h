#pragma once
#include "Drawable.h"
#include "JsonSerializable.h"

class BaseMesh : public Drawable, public JsonSerializable {
public:
	BaseMesh();
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	json ToJson() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};