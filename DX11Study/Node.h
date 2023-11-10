#pragma once
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include <string>
#include <vector>
#include <unordered_map>

class Node {
	friend class Model;
	friend class MP;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG);
	void Submit(size_t channel, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
	int GetId() const noexcept;
	const bool IsSelected() const noexcept;
	void SetSelected(bool selected) noexcept;
	bool HasChildren() const noexcept;
	void Accept(class ModelProbe& probe);
	void Accept(class TechniqueProbe& probe);
	std::string GetName() const noexcept;
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
	bool selected = false;
};
