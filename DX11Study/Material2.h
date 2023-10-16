#pragma once
#include <assimp/scene.h>
#include <filesystem>
#include <string>
#include <optional>
#include "Graphics.h"
#include "Texture.h"

class Material2 {
public:
	Material2(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG);
public:
	std::string rootPath;
	std::string name;

	std::optional<std::shared_ptr<Bind::Texture>> difTexture;
	std::optional<std::shared_ptr<Bind::Texture>> specTexture;
	std::optional<std::shared_ptr<Bind::Texture>> nrmTexture;
	
	aiColor3D materialColor = { 0.45f, 0.45f, 0.85f };
	aiColor3D specularColor = { 0.18f, 0.18f, 0.18f };
	float gloss = 8.0f;
};