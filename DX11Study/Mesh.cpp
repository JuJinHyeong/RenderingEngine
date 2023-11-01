#include "Mesh.h"
#include "Channels.h"
#include "Vertex.h"
#include "DynamicConstant.h"
#include "Rasterizer.h"
#include "TransformCbuf.h"
#include "Blender.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Sampler.h"
#include "InputLayout.h"
#include "ConstantBufferEx.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "Vertex.h"
#include "BoneTransformCbuf.h"
#include <array>
#include <algorithm>
#include <unordered_map>

#include "Dump.h"

Mesh::Mesh(
	Graphics& gfx,
	const Material2& mat,
	const aiMesh& mesh,
	string_to_uint_map boneNameToIndex,
	std::vector<Bone>& boneMatrixes,
	float scale) noexcept(!IS_DEBUG)
	:
	Drawable(gfx, mat, mesh, scale),
	name(mesh.mName.C_Str()),
	tag(mat.rootPath + "%" + mesh.mName.C_Str()),
	vertices(mesh.mVertices, mesh.mNumVertices),
	textureCoords(mesh.mTextureCoords[0], mesh.mNumVertices),
	normals(mesh.mNormals, mesh.mNumVertices),
	tangents(mesh.mTangents, mesh.mNumVertices),
	bitangents(mesh.mBitangents, mesh.mNumVertices),
	colors(mesh.mColors[0], mesh.mNumVertices),
	faces(mesh.mFaces, mesh.mNumFaces),
	bones(mesh.mBones, mesh.mNumBones),
	boneIndex(mesh.mNumVertices),
	boneWeight(mesh.mNumVertices),
	boneMatrixesPtr(&boneMatrixes)
{
	using namespace Bind;

	for (auto bone : bones) {
		for (unsigned int i = 0u; i < bone->mNumWeights; i++) {
			auto vertexId = bone->mWeights[i].mVertexId;
			auto weight = bone->mWeights[i].mWeight;
			for (unsigned int j = 0; j < 5; j++) {
				assert(j != 4 && "error!");
				if (boneWeight[vertexId][j] == 0.0f) {
					boneIndex[vertexId][j] = boneNameToIndex[bone->mName.C_Str()]; //TODO: Find bone index
					boneWeight[vertexId][j] = weight;
					break;
				}
			}
		}
	}

	custom::VertexLayout vertexLayout;
	Dcb::RawLayout pscLayout;
	Dcb::RawLayout vscLayout;
	{
		Technique phong{ "Phong", channel::main };
		{
			Step step("lambertian");
			std::string shaderCode = "Phong";
			vertexLayout.Append(custom::VertexLayout::Position3D);
			vertexLayout.Append(custom::VertexLayout::Normal);
			bool hasTexture = false;
			bool hasAlpha = false;
			bool hasGlossAlpha = false;
			bool hasSkinned = false;
			{
				if (mat.difTexture.has_value()) {
					hasTexture = true;
					shaderCode += "Dif";
					vertexLayout.Append(custom::VertexLayout::Texture2D);
					if ((*mat.difTexture)->HasAlpha()) {
						shaderCode += "Msk";
						hasAlpha = true;
					}

					step.AddBindable(*mat.difTexture);
				}
				else {
					pscLayout.Add<Dcb::Float3>("materialColor");
				}
				step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
			}
			{
				if (mat.specTexture.has_value()) {
					hasTexture = true;
					hasGlossAlpha = (*mat.specTexture)->HasAlpha();
					shaderCode += "Spc";

					step.AddBindable(*mat.specTexture);

					vertexLayout.Append(custom::VertexLayout::Texture2D);

					pscLayout.Add<Dcb::Bool>("useGlossAlpha");
					pscLayout.Add<Dcb::Bool>("useSpecularMap");
				}
				pscLayout.Add<Dcb::Float3>("specularColor");
				pscLayout.Add<Dcb::Float>("specularWeight");
				pscLayout.Add<Dcb::Float>("specularGloss");
			}
			{
				if (mat.nrmTexture.has_value()) {
					hasTexture = true;
					shaderCode += "Nrm";
					step.AddBindable(*mat.nrmTexture);

					vertexLayout.Append(custom::VertexLayout::Texture2D);
					vertexLayout.Append(custom::VertexLayout::Tangent);
					vertexLayout.Append(custom::VertexLayout::Bitangent);

					pscLayout.Add<Dcb::Bool>("useNormalMap");
					pscLayout.Add<Dcb::Float>("normalMapWeight");
				}
			}
			{
				if (!bones.empty()) {
					hasSkinned = true;
					shaderCode += "Skin";

					vertexLayout.Append(custom::VertexLayout::BoneIndex);
					vertexLayout.Append(custom::VertexLayout::BoneWeight);

					step.AddBindable(std::make_unique<BoneTransformCbuf>(gfx));
				}
			}
			{
				step.AddBindable(std::make_unique<TransformCbuf>(gfx));
				step.AddBindable(Blender::Resolve(gfx, false));
				auto pvs = VertexShader::Resolve(gfx, shaderCode + "VS.cso");
				step.AddBindable(InputLayout::Resolve(gfx, vertexLayout, *pvs));
				step.AddBindable(std::move(pvs));
				step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "PS.cso"));
				if (hasTexture) {
					step.AddBindable(Bind::Sampler::Resolve(gfx));
				}

				Dcb::Buffer buf{ std::move(pscLayout) };
				if (auto r = buf["materialColor"]; r.Exists()) {
					aiColor3D color = mat.materialColor;
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buf["useGlossAlpha"].SetifExists(hasGlossAlpha);
				buf["useSpecularMap"].SetifExists(true);
				if (auto r = buf["specularColor"]; r.Exists()) {
					aiColor3D color = mat.specularColor;
					r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
				}
				buf["specularWeight"].SetifExists(1.0f);
				if (auto r = buf["specularGloss"]; r.Exists()) {
					r = mat.gloss;
				}
				buf["useNormalMap"].SetifExists(true);
				buf["normalMapWeight"].SetifExists(1.0f);
				step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
			}
			phong.AddStep(std::move(step));
		}
		AddTechnique(std::move(phong));
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
		AddTechnique(std::move(outline));
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
		AddTechnique(std::move(map));
	}
	// set drawables
	{
		auto v = custom::VertexBuffer{ vertexLayout, mesh };
		if (scale != 1.0f) {
			for (size_t i = 0u; i < v.Size(); i++) {
				DirectX::XMFLOAT3& pos = v[i].Attr<custom::VertexLayout::ElementType::Position3D>();
				pos.x *= scale;
				pos.y *= scale;
				pos.z *= scale;
			}
		}
		pVertices = Bind::VertexBuffer::Resolve(gfx, tag, std::move(v));
	}
	{
		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
		pIndices = Bind::IndexBuffer::Resolve(gfx, tag, std::move(indices));
	}
	{
		pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept(!IS_DEBUG)
	:
	Drawable(gfx, mat, mesh, scale)
{}

// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs) {
	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	for (auto& pb : bindPtrs) {
		AddBind(std::move(pb));
	}
	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat4x4(&transform);
}

const std::vector<DirectX::XMMATRIX>& Mesh::GetBoneTransforms() const noexcept
{
	std::vector<DirectX::XMMATRIX> v;
	v.resize(boneMatrixesPtr->size());
	std::transform(boneMatrixesPtr->begin(), boneMatrixesPtr->end(), v.begin(), [](Bone& bone) {
		return bone.GetFinalMatrixXM();
	});
	return v;
}

void Mesh::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Submit(channels);
}

const std::span<aiVector3D>& Mesh::GetVertices() const noexcept {
	return vertices;
}

const std::span<aiVector3D>& Mesh::GetTextureCoords() const noexcept {
	return textureCoords;
}

const std::span<aiVector3D>& Mesh::GetNormals() const noexcept {
	return normals;
}

const std::span<aiVector3D>& Mesh::GetTangents() const noexcept {
	return tangents;
}

const std::span<aiVector3D>& Mesh::GetBitangents() const noexcept {
	return bitangents;
}

const std::span<aiColor4D>& Mesh::GetColors() const noexcept {
	return colors;
}

const std::span<aiFace>& Mesh::GetFaces() const noexcept {
	return faces;
}

const std::span<aiBone*>& Mesh::GetBones() const noexcept {
	return bones;
}

const std::vector<std::array<unsigned int, 4>>& Mesh::GetBoneIndex() const noexcept {
	return boneIndex;
}

const std::vector<std::array<float, 4>>& Mesh::GetBoneWeight() const noexcept {
	return boneWeight;
}
