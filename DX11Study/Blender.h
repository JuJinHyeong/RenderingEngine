#pragma once
#include "Bindable.h"
#include <array>
#include <optional>

namespace Bind {
	class Blender : public Bindable {
	public:
		Blender(Graphics& gfx, bool blending, std::optional<float> factor = {});
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;

		void SetFactor(float factor) noexcept(!IS_DEBUG);
		float GetFactor() const noexcept(!IS_DEBUG);

		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending, std::optional<float> factors = {});
		static std::string GenerateUID(bool blending, std::optional<float> factors = {});
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
		bool blending;
		// alpha rgba factor
		std::optional<std::array<float, 4>> factors;
	};
}

