#include "Material.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG)
	:
	rootPath(path.string())
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "/";
	aiString texFileName;
	{
		aiString aiStringName;
		material.Get(AI_MATKEY_NAME, aiStringName);
		name = aiStringName.C_Str();
	}
	{
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) {
			difTexture = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 0u);
		}
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS) {
			specTexture = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1u);
		}
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS) {
			nrmTexture = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2u);
		}
	}
	material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
	material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	material.Get(AI_MATKEY_SHININESS, gloss);
}

Material::Material(
	Graphics& gfx,
	const std::string& name, 
	std::optional<const std::string> difTexturePath, 
	std::optional<const std::string> specTexturePath, 
	std::optional<const std::string> nrmTexturePath, 
	const DirectX::XMFLOAT3 difColor, 
	const DirectX::XMFLOAT3 specColor, 
	const float gloss
)
	:
	name(name),
	diffuseColor(difColor),
	specularColor(specColor),
	gloss(gloss)
{
	if (difTexturePath.has_value()) {
		difTexture = Bind::Texture::Resolve(gfx, difTexturePath.value(), 0u);
	}
	if (specTexturePath.has_value()) {
		specTexture = Bind::Texture::Resolve(gfx, specTexturePath.value(), 1u);
	}
	if (nrmTexturePath.has_value()) {
		nrmTexture = Bind::Texture::Resolve(gfx, nrmTexturePath.value(), 2u);
	}
}
