#include "Object.h"

DirectX::XMMATRIX& Object::GetTransform() noexcept
{
	return transform;
}

void Object::Transform(DirectX::XMVECTOR translation, DirectX::XMVECTOR quaternion, DirectX::XMVECTOR scale) noexcept
{
	transform = DirectX::XMMatrixScalingFromVector(scale) 
		* DirectX::XMMatrixRotationQuaternion(quaternion) 
		* DirectX::XMMatrixTranslationFromVector(translation);
}
