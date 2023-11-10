#include "TransformCbuf.h"

namespace Bind {
	TransformCbuf::TransformCbuf(Graphics& gfx, UINT slot) {
		if (!pVcbuf) {
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		pParent(&parent) 
	{
		if (!pVcbuf) {
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept {
		pParent = &parent;
	}

	void TransformCbuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG) {
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept {
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noexcept {
		const auto model = pParent->GetTransformXM();
		const auto modelView = model * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(gfx.GetCamera()),
			DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection()),
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
}
