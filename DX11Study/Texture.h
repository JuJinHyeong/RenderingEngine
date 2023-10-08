#pragma once
#include "Bindable.h"

namespace Bind {
	class Texture : public Bindable {
	public:
		Texture(Graphics& gfx, const std::string& path_in, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0u);
		static std::string GenerateUID(const std::string& path, UINT slot);
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;
	private:
		unsigned int slot = 0u;
	protected:
		bool hasAlpha = false;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}