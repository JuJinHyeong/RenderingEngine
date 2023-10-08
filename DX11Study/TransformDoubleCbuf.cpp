#include "TransformDoubleCbuf.h"

namespace Bind {
	TransformDoubleCbuf::TransformDoubleCbuf(Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		:
		TransformCbuf(gfx, parent, slotV) {
		if (!pPcbuf) {
			pPcbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
		}
	}

	void TransformDoubleCbuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG) {
		const Transforms tf = GetTransforms(gfx);
		// vertex shader
		TransformCbuf::UpdateBindImpl(gfx, tf);
		// pixel shader
		UpdateBindImpl(gfx, tf);
	}

	void TransformDoubleCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept {
		pPcbuf->Update(gfx, tf);
		pPcbuf->Bind(gfx);
	}

	std::unique_ptr<PixelConstantBuffer<TransformDoubleCbuf::Transforms>> TransformDoubleCbuf::pPcbuf;
}