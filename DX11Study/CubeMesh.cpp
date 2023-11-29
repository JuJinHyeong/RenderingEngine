#include "CubeMesh.h"
#include "IndexedTriangleList.h"
#include "Cube.h"
#include "Graphics.h"
#include "VertexShader.h"
#include "VertexBuffer.h"
#include "Sampler.h"
#include "Texture.h"
#include "InputLayout.h"
#include "PixelShader.h"
#include "Rasterizer.h"
#include "TransformCbuf.h"
#include "Topology.h"
#include "IndexBuffer.h"
#include "Channels.h"
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"
#include "Blender.h"

using json = nlohmann::json;

CubeMesh::CubeMesh(Graphics& gfx)
	:
	mat(gfx, "brickwall", "Images/brickwall.jpg"),
	BaseMesh()
{
	using namespace Bind;
	pModel = std::make_unique<IndexedTriangleList>(Cube::MakeIndependentTextured());
	pModel->SetNormalsIndependentFlat();
	const std::string geometryTag = "#cube"
		+ std::to_string(1.0f) + std::to_string(1.0f) + std::to_string(1.0f);
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, pModel->vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, pModel->indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	SetMaterial(gfx, mat);
}

json CubeMesh::ToJson() const noexcept {
	json j;
	j["type"] = "cube";
	j["material"] = mat.ToJson();
	return j;
}

void CubeMesh::SetMaterial(Graphics& gfx, const Material& mat) noexcept
{
	using namespace Bind;
	techniques.clear();
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
}

const Material& CubeMesh::GetMaterial() const noexcept {
	return mat;
}

void CubeMesh::Modify(Graphics& gfx, const json& modifiedMesh) noexcept
{
	std::string modifiedMaterialName = modifiedMesh["material"]["name"];
	if (modifiedMaterialName != mat.name) {
		// change to map...
		if (modifiedMaterialName == "grass") {
			SetMaterial(gfx, Material{ gfx, "grass", "Images/grass.jpg" });
		}
		else if (modifiedMaterialName == "brickwall") {
			SetMaterial(gfx, Material{ gfx, "brickwall", "Images/brickwall.jpg" });
		}
		else if (modifiedMaterialName == "stone") {
			SetMaterial(gfx, Material{ gfx, "stone", "Images/stone.jpg" });
		}
	}
}
