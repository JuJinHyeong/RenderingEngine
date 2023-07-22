#include "Pyramid.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cone.h"

Pyramid::Pyramid(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& tdist)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized()) {

		auto pvs = std::make_unique<VertexShader>(gfx, L"BlendedColorVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"BlendedColorPS.cso"));


		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
			{ "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant {
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2] = { 0.0f, 0.0f };
		} pixelConstantBuffer;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pixelConstantBuffer, 1u));
	}
	struct Vertex {
		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 n;
		char color[4];
		char padding;
	};
	auto tesselation = tdist(rng);
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>(tesselation);
	// set vertex colors for mesh
	for (auto& v : model.vertices) {
		v.color[0] = 10;
		v.color[1] = 10;
		v.color[2] = (char)255;
		v.color[3] = 0;
	}
	for (int i = 0; i < tesselation; i++) {
		model.vertices[i * 3].color[0] = (char)255;
		model.vertices[i * 3].color[1] = 10;
		model.vertices[i * 3].color[2] = 10;
	}
	// deform mesh linearly
	model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	model.SetNormalsIndependentFlat();

	AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}
