#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {
	namespace wrl = Microsoft::WRL;

	Texture::Texture(Graphics& gfx, const std::string& path_in, UINT slot_in)
		: 
		path(path_in),
		slot(slot_in)
	{
		INFOMAN(gfx);

		const auto s = Surface::FromFile(path_in);
		hasAlpha = s.AlphaLoaded();

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		// not use mipmapping.
		textureDesc.MipLevels = 0;
		// array of texture.
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		// no anti aliasing
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		wrl::ComPtr<ID3D11Texture2D> pTexture;
		// mipmap 을 쓰기 위해서, subresource 를 처음부터 채우지 않고 업데이트를 하는 방식으로 사용한다.
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));
		// top miplevel 에 해당하는 값으로 업데이트 한다.
		GetContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0u);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		// not use mipmapping.
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

		GetContext(gfx)->GenerateMips(pTextureView.Get());
	}

	void Texture::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot) {
		return Codex::Resolve<Texture>(gfx, path, slot);
	}

	std::string Texture::GenerateUID(const std::string& path, UINT slot) {
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#"s + std::to_string(slot);
	}

	std::string Texture::GetUID() const noexcept {
		return GenerateUID(path, slot);
	}
	bool Texture::HasAlpha() const noexcept {
		return hasAlpha;
	}
}