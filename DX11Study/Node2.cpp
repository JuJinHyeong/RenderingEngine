#include "Node2.h"
#include "json.hpp"
using json = nlohmann::json;

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

json Node2::ToJson() const noexcept {
	json j = SceneObject2::ToJson();

	json meshes = json::array();
	for (const auto& meshPtr : meshPtrs) {
		const auto& mesh = std::dynamic_pointer_cast<Mesh>(meshPtr);
		if(mesh != nullptr) {
			meshes.push_back(mesh->ToJson());
		}
	}
	j["meshes"] = meshes;

	return j;
}

void Node2::Submit(size_t channel) const noexcept(!IS_DEBUG) {
	Submit(channel, DirectX::XMMatrixIdentity());
}

void Node2::Submit(size_t channel, DirectX::FXMMATRIX acuumulatedTransform) const noexcept(!IS_DEBUG) {
	auto built = acuumulatedTransform * relativeTransform * localTransform;
	for (const auto& pm : meshPtrs) {
		const auto& mesh = std::dynamic_pointer_cast<Mesh>(pm);
		if (mesh != nullptr) {
			mesh->Submit(channel, built);
		}
	}
	for (const auto& pc : childPtrs) {
		const auto& node = std::dynamic_pointer_cast<Node2>(pc);
		if (node != nullptr) {
			node->Submit(channel, built);
		}
	}
}

const DirectX::XMMATRIX& Node2::GetRelativeTransform() const noexcept
{
	return relativeTransform;
}
