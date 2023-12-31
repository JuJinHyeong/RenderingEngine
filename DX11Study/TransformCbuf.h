#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

namespace Bind {
	class TransformCbuf : public Bindable {
	protected:
		struct Transforms {
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};
	public:
		TransformCbuf(Graphics& gfx, UINT slot = 0u);
		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		void InitializeParentReference(const Drawable& parent) noexcept override;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
	};
}
