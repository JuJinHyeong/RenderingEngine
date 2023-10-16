#include "Material2.h"

Material2::Material2(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG)
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
	material.Get(AI_MATKEY_COLOR_DIFFUSE, materialColor);
	material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
	material.Get(AI_MATKEY_SHININESS, gloss);
}
