#pragma once
#include <assimp/scene.h>
#include <vector>
#include <filesystem>
#include "BindableCommon.h"
#include "Technique.h"
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"

class Material {
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG);
	// why extract in material??
	custom::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG);
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG);
	std::vector<Technique> GetTechniques() const noexcept;
private:
	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
private:
	custom::VertexLayout vertexLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};