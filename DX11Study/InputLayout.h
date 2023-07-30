#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include <vector>
#include <string>

namespace Bind {
	class InputLayout : public Bindable {
	public:
		InputLayout(Graphics& gfx, custom::VertexLayout layout, ID3DBlob* pVertexShaderByteCode);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const custom::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);
		static std::string GenerateUID(const custom::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr);
		std::string GetUID() const noexcept override;
	protected:
		custom::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}