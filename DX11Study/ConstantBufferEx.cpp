#include "ConstantBufferEx.h"
#include "GraphicsThrowMacros.h"
#include <cassert>

namespace Bind {
	void PixelConstantBufferEx::Update(Graphics& gfx, const Dcb::Buffer& buf) {
		assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
		memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}

	void PixelConstantBufferEx::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
	}

	PixelConstantBufferEx::PixelConstantBufferEx(Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf) 
		:
		slot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
		cbd.StructureByteStride = 0u;

		if (pBuf != nullptr) {
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = pBuf->GetData();
			float* a = (float*)csd.pSysMem;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}
		else {
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	}

	CachingPixelConstantBufferEx::CachingPixelConstantBufferEx(Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot) 
		:
		PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
		buf(Dcb::Buffer(layout))
	{}

	CachingPixelConstantBufferEx::CachingPixelConstantBufferEx(Graphics & gfx, const Dcb::Buffer & buf_in, UINT slot) 
		:
		PixelConstantBufferEx(gfx, buf_in.GetRootLayoutElement(), slot, &buf_in),
		buf(buf_in)
	{}

	const Dcb::LayoutElement& CachingPixelConstantBufferEx::GetRootLayoutElement() const noexcept {
		return buf.GetRootLayoutElement();
	}

	const Dcb::Buffer& CachingPixelConstantBufferEx::GetBuffer() const noexcept {
		return buf;
	}

	const void CachingPixelConstantBufferEx::SetBuffer(const Dcb::Buffer& buf_in) {
		buf.CopyFrom(buf_in);
		dirty = true;
	}

	void CachingPixelConstantBufferEx::Bind(Graphics& gfx) noexcept {
		if (dirty) {
			Update(gfx, buf);
			dirty = false;
		}
		PixelConstantBufferEx::Bind(gfx);
	}

	void CachingPixelConstantBufferEx::Accept(TechniqueProbe& probe) {
		if (probe.VisitBuffer(buf)) {
			dirty = true;
		}
	}

	NocachePixelConstantBufferEx::NocachePixelConstantBufferEx(Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot) 
		:
		PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr),
		pLayoutRoot(layout.ShareRoot())
	{}
	NocachePixelConstantBufferEx::NocachePixelConstantBufferEx(Graphics & gfx, const Dcb::Buffer & buf, UINT slot) 
		:
		PixelConstantBufferEx(gfx, buf.GetRootLayoutElement(), slot, &buf),
		pLayoutRoot(buf.ShareLayoutRoot())
	{}
	const Dcb::LayoutElement& NocachePixelConstantBufferEx::GetRootLayoutElement() const noexcept {
		return *pLayoutRoot;
	}

}
