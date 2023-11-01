#pragma once
#include <assimp/scene.h>
#include <DirectXMath.h>

class Bone {
public:
	Bone(const aiBone& bone);
	
	const DirectX::XMMATRIX GetOffsetMatrixXM() const noexcept;
	const DirectX::XMFLOAT4X4& GetOffsetMatrix() const noexcept;

	const DirectX::XMMATRIX GetFinalMatrixXM() const noexcept;
	const DirectX::XMFLOAT4X4& GetFinalMatrix() const noexcept;

	void SetFinalMatrix(DirectX::FXMMATRIX& mat);
	void SetFinalMatrix(DirectX::XMFLOAT4X4& mat);

private:
	DirectX::XMFLOAT4X4 offsetMatrix;
	DirectX::XMFLOAT4X4 finalMatrix;
};