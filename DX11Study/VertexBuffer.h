#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <vector>

namespace Bind {
	class VertexBuffer : public Bindable {
	public:
		VertexBuffer(Graphics& gfx, const std::string& tag, const custom::VertexBuffer& vbuf);
		VertexBuffer(Graphics& gfx, const custom::VertexBuffer& vbuf);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag, const custom::VertexBuffer& vbuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore) {
			return _GenerateUID(tag);
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string _GenerateUID(const std::string& tag);
	private:
		std::string tag;
		const UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
}