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

CubeMesh::CubeMesh(Graphics& gfx, float size)
	:
	BaseMesh(gfx)
{
	using namespace Bind;
	IndexedTriangleList model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const std::string geometryTag = "#cube" + std::to_string(size);
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto tcb = std::make_shared<TransformCbuf>(gfx);
	{
		Technique phong("Phong", channel::main);
		{
			Step only("lambertian");

			only.AddBindable(Texture::Resolve(gfx, "Images/brickwall.jpg"));
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "ShadowTestVS.cso");
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "ShadowTestPS.cso"));

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float3>("specularColor");
			lay.Add<Dcb::Float>("specularWeight");
			lay.Add<Dcb::Float>("specularGloss");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["specularColor"] = DirectX::XMFLOAT3{ 1.0f,0.3f,0.3f };
			buf["specularWeight"] = 0.001f;
			buf["specularGloss"] = 1.0f;
			only.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));


			only.AddBindable(Rasterizer::Resolve(gfx, false));

			only.AddBindable(tcb);

			phong.AddStep(std::move(only));
		}
		AddTechnique(std::move(phong));
	}

	{
		Technique outline("Outline", channel::main);
		{
			Step mask("outlineMask");

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "SolidVS.cso")));

			mask.AddBindable(std::move(tcb));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			Dcb::RawLayout lay;
			lay.Add<Dcb::Float4>("color");
			auto buf = Dcb::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
			draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "SolidVS.cso")));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

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
			draw.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), *VertexShader::Resolve(gfx, "SolidVS.cso")));

			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			map.AddStep(std::move(draw));
		}
		AddTechnique(std::move(map));
	}
}
