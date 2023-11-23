#include "BaseMesh.h"
#include <DirectXMath.h>

using json = nlohmann::json;

BaseMesh::BaseMesh()
{
	DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixIdentity());
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

json BaseMesh::ToJson() const noexcept {
	json j;
	return j;
}
