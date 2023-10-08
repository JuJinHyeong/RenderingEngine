#include "Rasterizer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSlided) 
		:
		twoSlided(twoSlided)
	{
		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterizerDesc.CullMode = twoSlided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &pRasterizer));
	}
	
	void Rasterizer::Bind(Graphics& gfx) noexcept(!IS_DEBUG) {
		GetContext(gfx)->RSSetState(pRasterizer.Get());
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSlided) {
		return Codex::Resolve<Rasterizer>(gfx, twoSlided);
	}

	std::string Rasterizer::GenerateUID(bool twoSlided) {
		using namespace std::string_literals;
		return typeid(Rasterizer).name() + "#"s + (twoSlided ? "b"s : "n"s);
	}

	std::string Rasterizer::GetUID() const noexcept {
		return GenerateUID(twoSlided);
	}

}