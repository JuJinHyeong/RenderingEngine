#include "PixelShader.h"
#include "GraphicsThrowMacros.h"

namespace Bind {
	PixelShader::PixelShader(Graphics& gfx, const std::wstring& path) {
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pByteCodeBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pByteCodeBlob->GetBufferPointer(), pByteCodeBlob->GetBufferSize(), nullptr, &pPixelShader));
	}

	void PixelShader::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

	ID3DBlob* PixelShader::GetByteCode() const noexcept {
		return pByteCodeBlob.Get();
	}
}
