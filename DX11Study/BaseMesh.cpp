#include "BaseMesh.h"

BaseMesh::BaseMesh(Graphics& gfx)
{
}

DirectX::XMMATRIX BaseMesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

void BaseMesh::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Submit(channels);
}
