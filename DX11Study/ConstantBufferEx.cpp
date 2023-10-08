#include "ConstantBufferEx.h"
#include "GraphicsThrowMacros.h"
#include <cassert>

namespace Bind {
	void ConstantBufferEx::Update(Graphics& gfx, const Dcb::Buffer& buf)
	{
		assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}

	ConstantBufferEx::ConstantBufferEx(Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf)
		:
		slot(slot)
	{
		INFOMAN(gfx);
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
		cbd.StructureByteStride = 0u;

		if (pBuf != nullptr) {
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = pBuf->GetData();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}
		else {
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	}

	void PixelConstantBufferEx::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}

	void VertexConstantBufferEx::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}

}
