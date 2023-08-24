#pragma once
#include "Bindable.h"

namespace Bind {
	class Rasterizer : public Bindable {
	public:
		Rasterizer(Graphics& gfx, bool twoSlided);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSlided);
		static std::string GenerateUID(bool twoSlided);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool twoSlided;
	};
}
