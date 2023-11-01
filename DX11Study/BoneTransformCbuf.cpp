#include "BoneTransformCbuf.h"
#include <algorithm>

namespace Bind {
	BoneTransformCbuf::BoneTransformCbuf(Graphics& gfx, UINT slot)
		:
		pParent(nullptr)
	{
		if (!pVcbuf) {
			Dcb::RawLayout vscLayout;
			vscLayout.Add<Dcb::Array>("transforms");
			vscLayout["transforms"].Set<Dcb::Matrix>(32);

			Dcb::Buffer buf{ std::move(vscLayout) };
			pVcbuf = std::make_unique<CachingVertexConstantBufferEx>(gfx, buf, slot);
		}
	}
	
	BoneTransformCbuf::BoneTransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
		:
		pParent(&parent)
	{
		if (!pVcbuf) {
			Dcb::RawLayout vscLayout;
			vscLayout.Add<Dcb::Array>("transforms");
			vscLayout["transforms"].Set<Dcb::Matrix>(32);

			Dcb::Buffer buf{ std::move(vscLayout) };
			pVcbuf = std::make_unique<CachingVertexConstantBufferEx>(gfx, buf, slot);
		}
	}
	
	void BoneTransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}
	
	void BoneTransformCbuf::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		UpdateBindImpl(gfx, GetBoneTransforms());
	}
	
	void BoneTransformCbuf::UpdateBindImpl(Graphics& gfx, const Dcb::Buffer& buf) noexcept
	{
		pVcbuf->SetBuffer(buf);
		pVcbuf->Bind(gfx);
	}

	Dcb::Buffer BoneTransformCbuf::GetBoneTransforms() noexcept
	{
		std::vector<DirectX::XMFLOAT4X4> transforms = pParent->GetBoneTransforms();
		Dcb::Buffer buf = pVcbuf->GetBuffer();
		for (size_t i = 0; i < transforms.size(); i++) {
			buf["transforms"][i] = transforms[i];
		}
		return buf;
	}
}
