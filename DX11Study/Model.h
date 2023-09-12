#pragma once
#include "FrameCommander.h"
#include "Mesh.h"
#include "Node.h"
#include <string>
#include <filesystem>

class ModelWindow;

class Model {
public:
	Model(Graphics& gfx, const std::string& path, const float scale = 1.0f);
	~Model() noexcept(!IS_DEBUG);
	//void Draw(Graphics& gfx) const;
	void Submit(FrameCommander& frame) const noexcept(!IS_DEBUG);
	void Accept(class ModelProbe& probe);
	//void ShowWindow(const char* windowName) noexcept(!IS_DEBUG);
	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
private:
	// const aiMaterial* -> aiMaterial* that aiMaterial can't be modified
	// const aiMaterial* const -> aiMaterial can't be modified also aiMaterial* can't be modified
	// const aiMaterial* const* -> aiMaterial* is array that each element can't be modified aiMaterial, pointer
	std::unique_ptr<Node> ParseNode(int& curId, const aiNode& node, float scale);
private:
	float scale = 1.0f;
	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
	//std::unique_ptr<class ModelWindow> pWindow;
};