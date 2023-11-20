#include "Mesh.h"
#include "Channels.h"
#include "Vertex.h"
#include "DynamicConstant.h"
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
#include <array>
#include <algorithm>
#include <unordered_map>
#include "Rasterizer.h"

#include "Dump.h"

Mesh::Mesh(
	Graphics& gfx,
	const Material& mat,
	const aiMesh& mesh,
	float scale) noexcept(!IS_DEBUG)
	:
	Drawable(gfx, mat, mesh, scale),
	name(mesh.mName.C_Str()),
	tag(mat.rootPath + "%" + mesh.mName.C_Str()),
	material(&mat)
{
	using namespace Bind;

	InitializePerVertexData(mesh);
	SetMaterial(gfx, mat, scale);
}

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

void Mesh::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Submit(channels);
}

void Mesh::SetMaterial(Graphics& gfx, const Material& mat, const float scale) noexcept(!IS_DEBUG)
{
	using namespace Bind;
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
					r = mat.diffuseColor;
				}
				buf["useGlossAlpha"].SetifExists(hasGlossAlpha);
				buf["useSpecularMap"].SetifExists(true);
				if (auto r = buf["specularColor"]; r.Exists()) {
					r = mat.specularColor;
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
		auto v = custom::VertexBuffer{ vertexLayout, *this };
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
	pIndices = Bind::IndexBuffer::Resolve(gfx, tag, indices);
	pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

const std::vector<DirectX::XMFLOAT3>& Mesh::GetVertices() const noexcept {
	return vertices;
}

const std::vector<DirectX::XMFLOAT3>& Mesh::GetTextureCoords() const noexcept {
	return textureCoords;
}

const std::vector<DirectX::XMFLOAT3>& Mesh::GetNormals() const noexcept {
	return normals;
}

const std::vector<DirectX::XMFLOAT3>& Mesh::GetTangents() const noexcept {
	return tangents;
}

const std::vector<DirectX::XMFLOAT3>& Mesh::GetBitangents() const noexcept {
	return bitangents;
}

const std::vector<DirectX::XMFLOAT4>& Mesh::GetColors() const noexcept {
	return colors;
}

#define Initialize(mData, data)\
if(mesh.mData != nullptr){\
	data.reserve(mesh.mNumVertices);\
	for(unsigned int i=0; i<mesh.mNumVertices; i++) {\
		data.emplace_back(mesh.mData[i].x, mesh.mData[i].y, mesh.mData[i].z);\
	}\
}
void Mesh::InitializePerVertexData(const aiMesh& mesh) noexcept(!IS_DEBUG)
{
	Initialize(mVertices, vertices);
	Initialize(mTextureCoords[0], textureCoords);
	Initialize(mNormals, normals);
	Initialize(mTangents, tangents);
	Initialize(mBitangents, bitangents);
	if (mesh.mColors[0] != nullptr) {
		for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
			colors.emplace_back(mesh.mColors[0][i].r, mesh.mColors[0][i].g, mesh.mColors[0][i].b, mesh.mColors[0][i].a);
		}
	}
	if (mesh.mFaces != nullptr) {
		for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
			assert(mesh.mFaces[i].mNumIndices == 3);
			// TODO: what difference between below two?
			//auto a = *reinterpret_cast<const DirectX::XMUINT3*>(mesh.mFaces[i].mIndices);
			indices.push_back(static_cast<unsigned short>(mesh.mFaces[i].mIndices[0]));
			indices.push_back(static_cast<unsigned short>(mesh.mFaces[i].mIndices[1]));
			indices.push_back(static_cast<unsigned short>(mesh.mFaces[i].mIndices[2]));
		}
	}
}
#undef Initialize(mData, data)
