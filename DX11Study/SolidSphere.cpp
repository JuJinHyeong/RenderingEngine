#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"


SolidSphere::SolidSphere(Graphics& gfx, float radius) {
	namespace dx = DirectX;

	if (!IsStaticInitialized()) {
		struct Vertex {
			dx::XMFLOAT3 pos;
		};
		auto model = Sphere::Make<Vertex>();
		model.Transform(dx::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<Bind::VertexBuffer>(gfx, model.vertices));
		AddIndexBuffer(std::make_unique<Bind::IndexBuffer>(gfx, model.indices));

		auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<Bind::PixelShader>(gfx, L"SolidPS.cso"));

		struct PSColorConstant {
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding = 0.0f;
		} colorConst;
		AddStaticBind(std::make_unique<Bind::PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<Bind::InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else {
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept {}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept {
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}