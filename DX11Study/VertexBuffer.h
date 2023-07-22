#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include <vector>

class VertexBuffer : public Bindable {
public:
	template<class T>
	VertexBuffer(Graphics& gfx, const std::vector<T>& vertices)
		:
		stride(sizeof(T))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0u;
		vbd.MiscFlags = 0u;
		vbd.ByteWidth = UINT(sizeof(T) * vertices.size());
		vbd.StructureByteStride = sizeof(T);

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices.data();

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &sd, &pVertexBuffer));
	}
	void Bind(Graphics& gfx) noexcept override;
private:
	const UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};