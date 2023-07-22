#pragma once
#include "Bindable.h"
#include <string>

class PixelShader : public Bindable {
public:
	PixelShader(Graphics& gfx, const std::wstring& path);
	void Bind(Graphics& gfx) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pByteCodeBlob;
};