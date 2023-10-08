#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include <vector>
#include <string>

namespace Bind {
	class VertexShader;

	// input layout is combination of layout and shader byte code.
	// so if we resolve input layout that (pos, color), then we have (pos, color).
	// but if we resolve input layout that (pos), then we have only (pos) despite setting both pos and color.
	class InputLayout : public Bindable {
	public:
		InputLayout(Graphics& gfx, custom::VertexLayout layout, const VertexShader& vs);
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
		const custom::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx, const custom::VertexLayout& layout, const VertexShader& vs);
		static std::string GenerateUID(const custom::VertexLayout& layout, const VertexShader& vs);
		std::string GetUID() const noexcept override;
	protected:
		std::string vertexShaderUID;
		custom::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}