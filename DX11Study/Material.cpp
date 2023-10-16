#include "Material.h"
#include "BindableCommon.h"
#include "Channels.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG) 
	:
	modelPath(path.string())
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "/";
	{
		aiString aiStringName;
		material.Get(AI_MATKEY_NAME, aiStringName);
		name = aiStringName.C_Str();
	}
	{
		Technique phong{ "Phong", channel::main };

		Step step("lambertian");
		std::string shaderCode = "Phong";
		aiString texFileName;

		vertexLayout.Append(custom::VertexLayout::Position3D);
		vertexLayout.Append(custom::VertexLayout::Normal);
		// pixel shader constant buffer layout
		Dcb::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;
		// has diffuse texture
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS) {
				// TODO: remove try catch
				try {
					auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 0u);
					hasTexture = true;
					shaderCode += "Dif";
					vertexLayout.Append(custom::VertexLayout::Texture2D);
					if (tex->HasAlpha()) {
						hasAlpha = true;
						shaderCode += "Msk";
					}
					step.AddBindable(std::move(tex));
				}
				catch (BasicException e) {
					pscLayout.Add<Dcb::Float3>("materialColor");
				}
			}
			else {
				pscLayout.Add<Dcb::Float3>("materialColor");
			}
			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
		}
		// has specular map
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS) {
				hasTexture = true;
				shaderCode += "Spc";
				vertexLayout.Append(custom::VertexLayout::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1u);
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
				pscLayout.Add<Dcb::Bool>("useGlossAlpha");
				pscLayout.Add<Dcb::Bool>("useSpecularMap");
			}
			pscLayout.Add<Dcb::Float3>("specularColor");
			pscLayout.Add<Dcb::Float>("specularWeight");
			pscLayout.Add<Dcb::Float>("specularGloss");
		}
		// has normal map
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS) {
				hasTexture = true;
				shaderCode += "Nrm";
				vertexLayout.Append(custom::VertexLayout::Texture2D);
				vertexLayout.Append(custom::VertexLayout::Tangent);
				vertexLayout.Append(custom::VertexLayout::Bitangent);
				step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2u));
				pscLayout.Add<Dcb::Bool>("useNormalMap");
				pscLayout.Add<Dcb::Float>("normalMapWeight");
			}
		}
		// common
		{
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));
			step.AddBindable(Blender::Resolve(gfx, false));
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "VS.cso");
			step.AddBindable(InputLayout::Resolve(gfx, vertexLayout, *pvs));
			step.AddBindable(std::move(pvs));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "PS.cso"));
			if (hasTexture) {
				step.AddBindable(Bind::Sampler::Resolve(gfx));
			}
			Dcb::Buffer buf{std::move(pscLayout)};
			if (auto r = buf["materialColor"]; r.Exists()) {
				// default color value
				aiColor3D color = { 0.45f, 0.45f, 0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetifExists(hasGlossAlpha);
			buf["useSpecularMap"].SetifExists(true);
			if (auto r = buf["specularColor"]; r.Exists()) {
				aiColor3D color = { 0.18f, 0.18f, 0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetifExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists()) {
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetifExists(true);
			buf["normalMapWeight"].SetifExists(1.0f);
			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
		}
		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
	{
		Technique outline("Outline", channel::main, false);
		{
			Step mask("outlineMask");

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			mask.AddBindable(InputLayout::Resolve(gfx, vertexLayout, *pvs));
			mask.AddBindable(std::move(pvs));

			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			// these can be pass-constant (tricky due to layout issues)
			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			draw.AddBindable(InputLayout::Resolve(gfx, vertexLayout, *pvs));
			draw.AddBindable(std::move(pvs));

			// this can be pass-constant
			draw.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));
			{
				Dcb::RawLayout lay;
				lay.Add<Dcb::Float3>("materialColor");
				auto buf = Dcb::Buffer(std::move(lay));
				buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
				draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));
			}

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));
			
			outline.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(outline));
	}
	// shadow map technique
	{
		Technique map{ "ShadowMap", channel::shadow,true };
		{
			Step draw("shadowMap");

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, vertexLayout, *VertexShader::Resolve(gfx, "SolidVS.cso")));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			map.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(map));
	}
}

custom::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept {
	return { vertexLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept {
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noexcept(!IS_DEBUG) {
	auto v = ExtractVertices(mesh);
	if (scale != 1.0f) {
		for (size_t i = 0u; i < v.Size(); i++) {
			DirectX::XMFLOAT3& pos = v[i].Attr<custom::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(v));
}

std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG) {
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<Technique> Material::GetTechniques() const noexcept {
	return techniques;
}

std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept {
	return modelPath + "%" + mesh.mName.C_Str();
}
