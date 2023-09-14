#pragma once
#include "Bindable.h"
#include "DynamicConstant.h"
#include "TechniqueProbe.h"

namespace Bind {
	class ConstantBufferEx : public Bindable {
	public:
		void Update(Graphics& gfx, const Dcb::Buffer& buf);
		virtual const Dcb::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		ConstantBufferEx(Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuf);
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};


	class PixelConstantBufferEx : public ConstantBufferEx {
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(Graphics& gfx) noexcept override;		
	};

	class VertexConstantBufferEx : public ConstantBufferEx {
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(Graphics& gfx) noexcept override;
	};

	template<class T>
	class CachingConstantBufferEx : public T {
	public:
		CachingConstantBufferEx(Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot) 
			:
			T(gfx, *layout.ShareRoot(), slot, nullptr),
			buf(Dcb::Buffer(layout))
		{}
		CachingConstantBufferEx(Graphics& gfx, const Dcb::Buffer& buf, UINT slot)
			:
			T(gfx, buf.GetRootLayoutElement(), slot, &buf),
			buf(buf)
		{}
		const Dcb::LayoutElement& GetRootLayoutElement() const noexcept override {
			return buf.GetRootLayoutElement();
		}
		const Dcb::Buffer& GetBuffer() const noexcept {
			return buf;
		}
		void SetBuffer(const Dcb::Buffer& buf_in) {
			buf.CopyFrom(buf_in);
			dirty = true;
		}
		void Bind(Graphics& gfx) noexcept override {
			if (dirty) {
				T::Update(gfx, buf);
				dirty = false;
			}
			T::Bind(gfx);
		}
		void Accept(TechniqueProbe& probe) override {
			if (probe.VisitBuffer(buf)) {
				dirty = true;
			}
		}
	private:
		bool dirty = false;
		Dcb::Buffer buf;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;

}