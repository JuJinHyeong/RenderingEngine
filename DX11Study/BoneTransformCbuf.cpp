#include "BoneTransformCbuf.h"

namespace Bind {
	BoneTransformCbuf::BoneTransformCbuf(Graphics& gfx, UINT slot)
	{
		if (!pVcbuf) {
			pVcbuf = std::make_unique<VertexConstantBuffer<BoneTransforms>>(gfx, slot);
		}
	}
	
	BoneTransformCbuf::BoneTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		pParent(&parent)
	{
		if (!pVcbuf) {
			pVcbuf = std::make_unique<VertexConstantBuffer<BoneTransforms>>(gfx, slot);
		}
	}
	
	void BoneTransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}
	
	void BoneTransformCbuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		UpdateBindImpl(gfx, GetBoneTransforms(gfx));
	}
	
	void BoneTransformCbuf::UpdateBindImpl(Graphics& gfx, const BoneTransforms& btf) noexcept
	{
		pVcbuf->Update(gfx, btf);
		pVcbuf->Bind(gfx);
	}

	BoneTransformCbuf::BoneTransforms BoneTransformCbuf::GetBoneTransforms(Graphics& gfx) noexcept
	{
		BoneTransforms boneTransforms;
		const auto& transforms = pParent->GetBoneTransforms();
		for (size_t i = 0; i < transforms.size(); i++) {
			boneTransforms.transforms[i] = transforms[i];
		}
		return boneTransforms;
	}

	std::unique_ptr<VertexConstantBuffer<BoneTransformCbuf::BoneTransforms>> BoneTransformCbuf::pVcbuf;
}
