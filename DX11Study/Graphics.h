#pragma once
#include "CustomWin.h"
#include "BasicException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <random>

namespace Bind {
	class Bindable;
	class RenderTarget;
}

class DepthStencil;

class Graphics {
	friend class GraphicsResource;
public:
	class Exception : public BasicException {
		using BasicException::BasicException;
	};
	class HrException : public Exception {
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception {
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException {
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

public:
	Graphics(HWND hWnd, unsigned int width, unsigned int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;

	std::shared_ptr<Bind::RenderTarget> GetTarget();
private:
	UINT width;
	UINT height;
	bool imguiEnabled = true;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	// why com ptr? not unique pointer
	// reason 1. unique pointer encapsulated. so pp is not good to access
	// reason 2. call release is needed.
	// reason 3. unique pointer is not shared pointer.
	// reason 4. comptr & operator overloaded.
	// caution. & operator is same as ReleaseAndGetAddressOf() function. don't forget release first and return pp
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::shared_ptr<Bind::RenderTarget> pTarget;
};
