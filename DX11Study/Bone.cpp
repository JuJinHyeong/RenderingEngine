#include "Bone.h"
#include "ExtendedXMMath.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>

Bone::Bone(const aiBone& bone)
	:
	name(bone.mName.C_Str()),
	offset(ConvertToDirectXMatrix(bone.mOffsetMatrix))
{
	for (unsigned int i = 0; i < bone.mNumWeights; i++) {
		aiVertexWeight vw = bone.mWeights[i];
		vertexWeights.push_back(std::make_pair(vw.mVertexId, vw.mWeight));
	}
}

const DirectX::XMFLOAT4X4& Bone::GetOffsetMatrix() const noexcept {
	return offset;
}

const std::string& Bone::GetName() const noexcept {
	return name;
}

const std::vector<std::pair<unsigned int, float>>& Bone::GetVertexWeight() const noexcept {
	return vertexWeights;
}
