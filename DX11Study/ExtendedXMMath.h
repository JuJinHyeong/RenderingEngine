#pragma once
#include <DirectXMath.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix);
DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix);
DirectX::XMMATRIX ScaleTranslation(DirectX::XMMATRIX matrix, float scale);
DirectX::XMFLOAT4X4 ConvertToDirectXMatrix(const aiMatrix4x4& aiMat);

DirectX::XMFLOAT3 GetPositionFromMatrix(const DirectX::FXMMATRIX& mat);
DirectX::XMFLOAT4 GetRotationFromMatrix(const DirectX::FXMMATRIX& mat);
DirectX::XMFLOAT3 GetScaleFromMatrix(const DirectX::FXMMATRIX& mat);