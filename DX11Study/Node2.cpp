#include "Node2.h"

Node2::Node2(const aiNode& node, const std::vector<std::shared_ptr<Mesh>>& modelMeshPtrs)
	:
	relativeTransform(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)),
	SceneObject2(node.mName.C_Str())
{
	type = node.mNumMeshes == 0 ? Type::empty : Type::mesh;

	for (unsigned int i = 0; i < node.mNumMeshes; i++) {
		const auto meshIdx = node.mMeshes[i];
		meshPtrs.push_back(modelMeshPtrs.at(meshIdx));
	}

	for (unsigned int i = 0; i < node.mNumChildren; i++) {
		childPtrs.push_back(std::make_shared<Node2>(*node.mChildren[i], modelMeshPtrs));
	}
}

void Node2::Submit(size_t channel, DirectX::FXMMATRIX acuumulatedTransform) const noexcept(!IS_DEBUG) {
	auto built = acuumulatedTransform * relativeTransform * localTransform;
	for (const auto& pm : meshPtrs) {
		pm->Submit(channel, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(channel, built);
	}
}
