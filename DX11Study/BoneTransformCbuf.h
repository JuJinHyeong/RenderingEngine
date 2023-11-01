#pragma once
#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Drawable.h"

namespace Bind {
	class BoneTransformCbuf : public Bindable {
	private:
		struct BoneTransforms {
			DirectX::XMMATRIX transforms[32];
		};
	public:
		BoneTransformCbuf(Graphics& gfx, UINT slot = 2u);
		BoneTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 2u);
		void InitializeParentReference(const Drawable& parent) noexcept override;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	private:
		void UpdateBindImpl(Graphics& gfx, const BoneTransforms& tf) noexcept;
		BoneTransforms GetBoneTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<BoneTransforms>> pVcbuf;
		const Drawable* pParent;
	};
}
