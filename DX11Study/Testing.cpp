#include "Testing.h"
#include "Vertex.h"
#include "ExtendedXMMath.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "Surface.h"

void TestDynamicMeshLoading() {
	using namespace custom;
	Assimp::Importer imp;

	const auto pScene = imp.ReadFile("Models/brick_wall/brick_wall.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	auto layout = VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D);
	VertexBuffer buf{ std::move(layout), *pScene->mMeshes[0] };

	for (auto i = 0ull, end = buf.Size(); i < end; i++) {
		const auto a = buf[i].Attr<VertexLayout::Position3D>();
		const auto b = buf[i].Attr<VertexLayout::Normal>();
		const auto c = buf[i].Attr<VertexLayout::Tangent>();
		const auto d = buf[i].Attr<VertexLayout::Bitangent>();
		const auto e = buf[i].Attr<VertexLayout::Texture2D>();
	}
}


void TestScaleMatrixTranslation()
{
	auto tlMat = DirectX::XMMatrixTranslation(20.f, 30.f, 40.f);
	tlMat = ScaleTranslation(tlMat, 0.1f);
	DirectX::XMFLOAT4X4 f4;
	DirectX::XMStoreFloat4x4(&f4, tlMat);
	auto etl = ExtractTranslation(f4);
	assert(etl.x == 2.f && etl.y == 3.f && etl.z == 4.f);
}


void D3DTestScratchPad(Window& wnd) {
	namespace dx = DirectX;
	using namespace custom;

	const auto RenderWithVS = [&gfx = wnd.Gfx()](const std::string& vsName) {
		const auto bitop = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const auto layout = VertexLayout{}
			.Append(VertexLayout::Position2D)
			.Append(VertexLayout::Float3Color);

		VertexBuffer vb(layout, 3);
		vb[0].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.0f,0.5f };
		vb[0].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 1.0f,0.0f,0.0f };
		vb[1].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ 0.5f,-0.5f };
		vb[1].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,1.0f,0.0f };
		vb[2].Attr<VertexLayout::Position2D>() = dx::XMFLOAT2{ -0.5f,-0.5f };
		vb[2].Attr<VertexLayout::Float3Color>() = dx::XMFLOAT3{ 0.0f,0.0f,1.0f };
		const auto bivb = Bind::VertexBuffer::Resolve(gfx, "##?", vb);

		const std::vector<unsigned short> idx = { 0,1,2 };
		const auto biidx = Bind::IndexBuffer::Resolve(gfx, "##?", idx);

		const auto bips = Bind::PixelShader::Resolve(gfx, "TestPS.cso");

		const auto bivs = Bind::VertexShader::Resolve(gfx, vsName);
		const auto bilay = Bind::InputLayout::Resolve(gfx, layout, *bivs);

		auto rt = Bind::ShaderInputRenderTarget{ gfx,1280,720,0 };

		biidx->Bind(gfx);
		bivb->Bind(gfx);
		bitop->Bind(gfx);
		bips->Bind(gfx);
		bivs->Bind(gfx);
		bilay->Bind(gfx);
		rt.Clear(gfx, { 0.0f,0.0f,0.0f,1.0f });
		rt.BindAsBuffer(gfx);
		gfx.DrawIndexed(biidx->GetCount());
		gfx.GetTarget()->BindAsBuffer(gfx);
		rt.ToSurface(gfx).Save("Test_" + vsName + ".png");
	};

	RenderWithVS("Test1VS.cso");
	RenderWithVS("Test2VS.cso");
}