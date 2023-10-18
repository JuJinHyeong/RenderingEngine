#include "Bone.h"
#include "ExtendedXMMath.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>

Bone::Bone(const unsigned int meshIndex, const aiBone& bone)
	:
	name(bone.mName.C_Str()),
	meshIndex(meshIndex),
	offset(ConvertToDirectXMatrix(bone.mOffsetMatrix)),
	vertexWeights(bone.mNumWeights)
{
	for (unsigned int i = 0; i < bone.mNumWeights; i++) {
		aiVertexWeight vw = bone.mWeights[i];
		vertexWeights[i] = std::move(std::make_pair(vw.mVertexId, vw.mWeight));
	}
}

const DirectX::XMFLOAT4X4& Bone::GetOffsetMatrix() const noexcept {
	return offset;
}

const std::string& Bone::GetName() const noexcept {
	return name;
}

const unsigned int Bone::GetMeshIndex() const noexcept
{
	return meshIndex;
}

const std::vector<std::pair<unsigned int, float>>& Bone::GetVertexWeight() const noexcept {
	return vertexWeights;
}
