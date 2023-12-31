#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {
	template<typename T>
	class ConstantBuffer : public Bindable {
	public:
		ConstantBuffer(Graphics& gfx, const T& consts, UINT slot = 0u)
			:
			slot(slot) {
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd = {};
			// dynamic usage allow the use of the lock function. it is also possible to update with updatedsubresource on a non-dynamic.
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			// not array.
			cbd.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;

			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		};

		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			:
			slot(slot) {
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd = {};
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(T);
			cbd.StructureByteStride = 0u;

			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}

		void Update(Graphics& gfx, const T& consts) {
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot;
	};

	template<typename T>
	class VertexConstantBuffer : public ConstantBuffer<T> {
		using ConstantBuffer<T>::slot;
	public:
		using ConstantBuffer<T>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override {
			Bindable::GetContext(gfx)->VSSetConstantBuffers(slot, 1u, ConstantBuffer<T>::pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const T& consts, UINT slot = 0) {
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0) {
			return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const T&, UINT slot) {
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot = 0) {
			using namespace std::string_literals;
			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override {
			return GenerateUID(slot);
		}
	};

	template<typename T>
	class PixelConstantBuffer : public ConstantBuffer<T> {
		using ConstantBuffer<T>::slot;
		using ConstantBuffer<T>::pConstantBuffer;
	public:
		using ConstantBuffer<T>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept(!IS_DEBUG) override {
			Bindable::GetContext(gfx)->PSSetConstantBuffers(slot, 1u, ConstantBuffer<T>::pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const T& consts, UINT slot = 0u) {
			return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, UINT slot = 0u) {
			return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const T& consts, UINT slot) {
			return GenerateUID(slot);
		}
		static std::string GenerateUID(UINT slot) {
			using namespace std::string_literals;
			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override {
			return GenerateUID(slot);
		}
	};
}