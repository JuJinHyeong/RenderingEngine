#pragma once
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

class Bone {
public:
	Bone(const std::string& meshName, const aiBone& bone);
	const DirectX::XMFLOAT4X4& GetOffsetMatrix() const noexcept;
	const std::string& GetName() const noexcept;
	const std::string& GetMeshName() const noexcept;
	const std::vector<std::pair<unsigned int, float>>& GetVertexWeight() const noexcept;
private:
	std::string name;
	std::string meshName;
	DirectX::XMFLOAT4X4 offset;
	std::vector<std::pair<unsigned int, float>> vertexWeights;
};