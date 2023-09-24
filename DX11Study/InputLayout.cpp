#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "VertexShader.h"

namespace Bind {
	InputLayout::InputLayout(Graphics& gfx, custom::VertexLayout layout_in, const VertexShader& vs)
		:
		layout(layout_in),
		vertexShaderUID(vs.GetUID())
	{
		INFOMAN(gfx);

		const auto d3dLayout = layout.GetD3DLayout();
		const auto pVertexShaderByteCode = vs.GetBytecode();

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

	const custom::VertexLayout InputLayout::GetLayout() const noexcept {
		return layout;
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, const custom::VertexLayout& layout, const VertexShader& vs) {
		return Codex::Resolve<InputLayout>(gfx, layout, vs);
	}

	std::string InputLayout::GenerateUID(const custom::VertexLayout& layout, const VertexShader& vs) {
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}

	std::string InputLayout::GetUID() const noexcept {
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
	}
}
