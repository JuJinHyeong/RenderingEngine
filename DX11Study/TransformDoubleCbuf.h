#pragma once
#include "TransformCbuf.h"

namespace Bind {
	class TransformDoubleCbuf : public TransformCbuf {
	public:
		TransformDoubleCbuf(Graphics& gfx, const Drawable& parent, UINT slotV = 0u, UINT slotP = 0u);
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> pPcbuf;
	};
}
