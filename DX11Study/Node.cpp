#include "Node.h"
#include "imgui/imgui.h"
#include "ModelProbe.h"

// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	:
	meshPtrs(std::move(meshPtrs)),
	name(name),
	id(id) {
	DirectX::XMStoreFloat4x4(&this->transform, transform);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

int Node::GetId() const noexcept {
	return id;
}

bool Node::HasChildren() const noexcept {
	return !childPtrs.empty();
}

// dfs
void Node::Accept(ModelProbe& probe) {
	if (probe.PushNode(*this)) {
		for (auto& cp : childPtrs) {
			cp->Accept(probe);
		}
		probe.PopNode(*this);
	}
}

void Node::Accept(TechniqueProbe& probe) {
	for (auto& mp : meshPtrs) {
		mp->Accept(probe);
	}
}

std::string Node::GetName() const noexcept {
	return name;
}

void Node::Submit(size_t channel, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	const auto built = DirectX::XMLoadFloat4x4(&appliedTransform)
		* DirectX::XMLoadFloat4x4(&transform)
		* accumulatedTransform;

	for (const auto pm : meshPtrs) {
		pm->Submit(channel, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(channel, built);
	}
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept {
	return appliedTransform;
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG) {
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}
