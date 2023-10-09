#pragma once
#include "Bindable.h"

namespace Bind {
	class Stencil : public Bindable {
	public:
		enum class Mode {
			Off,
			Write,
			Mask,
			DepthOff,
			DepthReversed,
			DepthFirst,
		};
		Stencil(Graphics& gfx, Mode mode);
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
		static std::shared_ptr<Stencil> Resolve(Graphics& gfx, Mode mode);
		static std::string GenerateUID(Mode mode);
		std::string GetUID() const noexcept override;
	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
		Mode mode;
	};
}


