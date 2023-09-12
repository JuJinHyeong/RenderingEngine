#include "Node.h"
#include "imgui/imgui.h"
#include "FrameCommander.h"

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

void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	const auto built = DirectX::XMLoadFloat4x4(&appliedTransform)
		* DirectX::XMLoadFloat4x4(&transform)
		* accumulatedTransform;

	for (const auto pm : meshPtrs) {
		pm->Submit(frame, built);
	}
	for (const auto& pc : childPtrs) {
		pc->Submit(frame, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}
void Node::ShowTree(Node*& pSelectedNode) const noexcept(!IS_DEBUG) {
	// compare with this can't be??
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	const int nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| (pSelectedNode == this ? ImGuiTreeNodeFlags_Selected : 0)
		| (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	// create expandable node
	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)GetId(), nodeFlags, name.c_str());

	if (ImGui::IsItemClicked()) {
		// future need to modify
		pSelectedNode = const_cast<Node*>(this);
	}
	// int to void* need to cast.
	if (expanded) {
		for (const auto& pChild : childPtrs) {
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}
void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG) {
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}
