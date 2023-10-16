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
};