#include "PixelShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "CustomUtils.h"

namespace Bind {
	PixelShader::PixelShader(Graphics& gfx, const std::string& path) 
		:
		path(path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(ToWide("ShaderBins/" + path).c_str(), &pByteCodeBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pByteCodeBlob->GetBufferPointer(), pByteCodeBlob->GetBufferSize(), nullptr, &pPixelShader));
	}

	void PixelShader::Bind(Graphics& gfx) noexcept {
		GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

	ID3DBlob* PixelShader::GetByteCode() const noexcept {
		return pByteCodeBlob.Get();
	}
	std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, const std::string& path) {
		return Codex::Resolve<PixelShader>(gfx, path);
	}
	std::string PixelShader::GenerateUID(const std::string& path) {
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept {
		return GenerateUID(path);
	}
}
