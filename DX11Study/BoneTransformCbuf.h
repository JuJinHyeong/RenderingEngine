#pragma once
#include "Bindable.h"
#include "ConstantBufferEx.h"
#include "Drawable.h"
#include "Bone.h"

namespace Bind {
	class BoneTransformCbuf : public Bindable {
	public:
		BoneTransformCbuf(Graphics& gfx, UINT slot = 2u);
		BoneTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 2u);
		void InitializeParentReference(const Drawable& parent) noexcept override;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	private:
		void UpdateBindImpl(Graphics& gfx, const Dcb::Buffer& btf) noexcept;
		Dcb::Buffer GetBoneTransforms() noexcept;
	private:
		const Drawable* pParent;
		std::unique_ptr<CachingVertexConstantBufferEx> pVcbuf;
	};
}
