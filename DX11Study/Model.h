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
	void Submit(size_t channel) noexcept(!IS_DEBUG);
	void Accept(class ModelProbe& probe);
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg);
private:
	std::unique_ptr<Node> ParseNode(int& curId, const aiNode& node, float scale, int space=0);

private:
	float scale = 1.0f;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};