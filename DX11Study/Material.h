#pragma once
#include <assimp/scene.h>
#include <filesystem>
#include <string>
#include <optional>
#include "Graphics.h"
#include "Texture.h"
#include "JsonSerializable.h"

class Material : public JsonSerializable {
public:
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG);
	Material(
		Graphics& gfx,
		const std::string& name,
		std::optional<const std::string> difTexturePath = std::nullopt,
		std::optional<const std::string> specTexturePath = std::nullopt,
		std::optional<const std::string> nrmTexturePath = std::nullopt,
		const DirectX::XMFLOAT3 difColor = { 0.45f, 0.45f, 0.85f },
		const DirectX::XMFLOAT3 specColor = { 0.18f, 0.18f, 0.18f },
		const float gloss = 8.0f
	);
	json ToJson() const noexcept override;
public:
	std::string rootPath;
	std::string name;

	std::optional<std::shared_ptr<Bind::Texture>> difTexture = std::nullopt;
	std::optional<std::string> difTexturePath;
	std::optional<std::shared_ptr<Bind::Texture>> specTexture = std::nullopt;
	std::optional<std::string> specTexturePath;
	std::optional<std::shared_ptr<Bind::Texture>> nrmTexture = std::nullopt;
	std::optional<std::string> nrmTexturePath;

	DirectX::XMFLOAT3 diffuseColor = { 0.45f, 0.45f, 0.85f };
	DirectX::XMFLOAT3 specularColor = { 0.18f, 0.18f, 0.18f };
	float gloss = 8.0f;
};