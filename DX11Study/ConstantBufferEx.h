#pragma once
#include "Bindable.h"
#include "DynamicConstant.h"
#include "TechniqueProbe.h"

namespace Bind {
	class PixelConstantBufferEx : public Bindable {
	public:
		void Update(Graphics& gfx, const Dcb::Buffer& buf);
		void Bind(Graphics& gfx) noexcept override;
		virtual const Dcb::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		PixelConstantBufferEx(Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf);
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	class CachingPixelConstantBufferEx : public PixelConstantBufferEx {
	public:
		CachingPixelConstantBufferEx(Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot);
		CachingPixelConstantBufferEx(Graphics& gfx, const Dcb::Buffer& buf_in, UINT slot);
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override;
		const Dcb::Buffer& GetBuffer() const noexcept;
		const void SetBuffer(const Dcb::Buffer& buf_in);
		void Bind(Graphics& gfx) noexcept override;
		void Accept(TechniqueProbe& probe) override;
	private:
		bool dirty = false;
		Dcb::Buffer buf;
	};

	class NocachePixelConstantBufferEx : public PixelConstantBufferEx {
	public:
		NocachePixelConstantBufferEx(Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot);
		NocachePixelConstantBufferEx(Graphics& gfx, const Dcb::Buffer& buf, UINT slot);
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override;
	private:
		std::shared_ptr<Dcb::LayoutElement> pLayoutRoot;
	};
}