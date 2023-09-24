#include "CameraIndicator.h"
#include "BindableCommon.h"

CameraIndicator::CameraIndicator(Graphics& gfx) {
	using namespace Bind;

	const auto geometryTag = "$cam";
	custom::VertexLayout layout;
	layout.Append(custom::VertexLayout::Position3D);
	custom::VertexBuffer vertices{ std::move(layout) };
	{
		const float x = 4.0f / 3.0f * 0.75f;
		const float y = 1.0f * 0.75f;
		const float z = -2.0f;
		const float thalf = x * 0.5f;
		const float tspace = y * 0.2f;
		vertices.EmplaceBack(DirectX::XMFLOAT3{ -x, y, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ x, y, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ x, -y, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ -x, -y, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f, 0.0f, z });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ -thalf, y + tspace, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ thalf, y + tspace, 0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f, y + tspace + thalf, 0.0f });
	}
	std::vector<unsigned short> indices;
	{
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(3);
		indices.push_back(0);

		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(1);
		indices.push_back(4);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(3);
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(5);
	}

	pVertices = VertexBuffer::Resolve(gfx, geometryTag, vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line;
		{
			Step only("lambertian");

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			only.AddBindable(InputLayout::Resolve(gfx, vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));

			struct PSColorConstant {
				DirectX::XMFLOAT3 color = { 0.2f,0.2f,0.6f };
				float padding = 0.0f;
			} colorConst;
			only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));

			only.AddBindable(std::make_shared<TransformCbuf>(gfx));

			only.AddBindable(Rasterizer::Resolve(gfx, false));

			line.AddStep(std::move(only));
		}
		AddTechnique(std::move(line));
	}
}

void CameraIndicator::SetPos(DirectX::XMFLOAT3 pos) noexcept {
	this->pos = pos;
}

void CameraIndicator::SetRotation(DirectX::XMFLOAT3 rot) noexcept {
	this->rot = rot;
}

DirectX::XMMATRIX CameraIndicator::GetTransformXM() const noexcept {
	return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rot)) *
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos));
}