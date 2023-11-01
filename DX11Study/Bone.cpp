#include "Bone.h"

Bone::Bone(const aiBone& bone)
	:
	finalMatrix(DirectX::XMFLOAT4X4())
{
	DirectX::XMMATRIX transposedMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&bone.mOffsetMatrix)));
	DirectX::XMStoreFloat4x4(&offsetMatrix, transposedMatrix);
}

const DirectX::XMMATRIX Bone::GetOffsetMatrixXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&offsetMatrix);
}

const DirectX::XMFLOAT4X4& Bone::GetOffsetMatrix() const noexcept
{
	return offsetMatrix;
}

const DirectX::XMMATRIX Bone::GetFinalMatrixXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&finalMatrix);
}

const DirectX::XMFLOAT4X4& Bone::GetFinalMatrix() const noexcept
{
	return finalMatrix;
}

void Bone::SetFinalMatrix(DirectX::FXMMATRIX& mat)
{
	DirectX::XMStoreFloat4x4(&finalMatrix, mat);
}

void Bone::SetFinalMatrix(DirectX::XMFLOAT4X4& mat)
{
	finalMatrix = mat;
}
