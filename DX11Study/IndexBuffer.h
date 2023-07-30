#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include <vector>

namespace Bind {
	class IndexBuffer : public Bindable {
	public:
		IndexBuffer(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices);
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;

		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore...ignore) {
			return _GenerateUID(tag);
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string _GenerateUID(const std::string& tag);
	private:
		std::string tag;
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}