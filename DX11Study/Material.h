#pragma once
#include <assimp/scene.h>
#include <vector>
#include "BindableCommon.h"
#include <filesystem>
#include "Technique.h"

class Material {
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG);
	custom::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<Technique> GetTechniques() const noexcept;
private:
	custom::VertexLayout vertexLayout;
	std::vector<Technique> techniques;
};