#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {
	InputLayout::InputLayout(Graphics& gfx, custom::VertexLayout layout_in, ID3DBlob* pVertexShaderByteCode)
		:
		layout(layout_in)
	{
		INFOMAN(gfx);

		const auto d3dLayout = layout.GetD3DLayout();

		GFX_THROW_INFO(
			GetDevice(gfx)->CreateInputLayout(
				d3dLayout.data(),
				static_cast<UINT>(d3dLayout.size()),
				pVertexShaderByteCode->GetBufferPointer(),
				pVertexShaderByteCode->GetBufferSize(),
				&pInputLayout
			)
		);
	}

	void InputLayout::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, const custom::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode) {
		return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderByteCode);
	}

	std::string InputLayout::GenerateUID(const custom::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode) {
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}

	std::string InputLayout::GetUID() const noexcept {
		return GenerateUID(layout);
	}
}
