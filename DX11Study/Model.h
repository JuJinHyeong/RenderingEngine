#pragma once
#include "Mesh.h"
#include "Node.h"
#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>
#include "Bone.h"

namespace Rgph {
	class RenderGraph;
}

class Model {
	friend class MP;
public:
	Model(Graphics& gfx, const std::string& path, const float scale = 1.0f, const bool test = false);
	~Model() noexcept(!IS_DEBUG);
	//void Draw(Graphics& gfx) const;
	void Submit(size_t channel) const noexcept(!IS_DEBUG);
	void Accept(class ModelProbe& probe);
	//void ShowWindow(const char* windowName) noexcept(!IS_DEBUG);
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg);
	const std::vector<Bone>& GetBoneMatrixes() const noexcept;
	float& GetAnimationTick() noexcept;
private:
	// const aiMaterial* -> aiMaterial* that aiMaterial can't be modified
	// const aiMaterial* const -> aiMaterial can't be modified also aiMaterial* can't be modified
	// const aiMaterial* const* -> aiMaterial* is array that each element can't be modified aiMaterial, pointer
	std::unique_ptr<Node> ParseNode(int& curId, const aiNode& node, float scale, int space=0);
	unsigned int ResolveBoneIndex(const aiBone& bone) noexcept(!IS_DEBUG);

private:
	float scale = 1.0f;
	std::unique_ptr<Node> pRoot;
	DirectX::XMMATRIX rootInverseTransform = DirectX::XMMatrixIdentity();
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	// TODO: vertex weight is not needed to save on model
	std::unordered_map<std::string, unsigned int> nameBoneIndexMap;
	std::vector<Bone> bones;
	std::vector<Animation> animations;
	float animationTick = 0.0f;
};