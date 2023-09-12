#pragma once
#include "Mesh.h"
#include <string>
#include <vector>

class Node {
	friend class Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
	int GetId() const noexcept;
	// index need to imgui, Node need to graphics
	//void ShowTree(Node*& pSelectedNode)  const noexcept(!IS_DEBUG);
private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
private:
	std::string name;
	int id;
	std::vector<std::unique_ptr<Node>> childPtrs;
	std::vector<Mesh*> meshPtrs;
	// relative transform from parent
	// loaded by file
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};
