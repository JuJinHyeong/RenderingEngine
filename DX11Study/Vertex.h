#pragma once
#include <assimp/scene.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
#include <type_traits>
#include <d3d11.h>

#define DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh,size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

#define LAYOUT_ELEMENT_TYPES \
	X( Position2D ) \
	X( Position3D ) \
	X( Texture2D ) \
	X( Normal ) \
	X( Tangent ) \
	X( Bitangent ) \
	X( Float3Color ) \
	X( Float4Color ) \
	X( BGRAColor ) \
	X( Count )

namespace custom {
	class VertexLayout {
	public:
		enum ElementType {
#define X(el) el,
			LAYOUT_ELEMENT_TYPES
#undef X
		};
		template<ElementType> struct Map;
		template<> struct Map<Position2D> {
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<Position3D> {
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<Texture2D> {
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0]);
		};
		template<> struct Map<Normal> {
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mNormals);
		};
		template<> struct Map<Tangent> {
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "T";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mTangents);
		};
		template<> struct Map<Bitangent> {
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "BT";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mBitangents);
		};
		template<> struct Map<Float3Color> {
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<Float4Color> {
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<BGRAColor> {
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		// for default case
		template<> struct Map<Count> {
			using SysType = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "!INVALID!";
			static constexpr const char* code = "!INV!";
			DYNAMIC_VERTEX_ELEMENT_AI_EXTRACTOR(mFaces)
		};

		// TODO: make to template lambda
		// dynamic type to static type
		template<template<VertexLayout::ElementType> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args) noexcept(!IS_DEBUG) {
			switch (type) {
#define X(el) case VertexLayout::el: return F<VertexLayout::el>::Exec( std::forward<Args>( args )... );
				LAYOUT_ELEMENT_TYPES
#undef X
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

		class Element {
		public:
			Element(ElementType type, size_t offset);
			size_t GetOffsetAfter() const noexcept(!IS_DEBUG);
			size_t GetOffset() const;
			size_t Size() const noexcept(!IS_DEBUG);
			static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG);
			ElementType GetType() const noexcept;
			const char* GetCode() const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG);
		private:
			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept {
				return { Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0 };
			}
		private:
			ElementType type;
			size_t offset;
		};
	public:
		template<ElementType Type>
		const Element& Resolve() const noexcept(!IS_DEBUG) {
			for (auto& e : elements) {
				if (e.GetType() == Type) {
					return e;
				}
			}
			assert("Could not resolve element type" && false);
			return elements.front();
		};
		const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG);
		VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG);
		size_t Size() const noexcept(!IS_DEBUG);
		std::string GetCode() const noexcept;
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG);
		template<ElementType Type>
		bool Has() const noexcept {
			for (auto& e : elements) {
				if (e.GetType() == Type) {
					return true;
				}
			}
			return false;
		}
	private:
		std::vector<Element> elements;
	};

	class Vertex {
		friend class VertexBuffer;
	private:
		template<VertexLayout::ElementType type>
		struct AttributeSetting {
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val) noexcept(!IS_DEBUG) {
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr() noexcept(!IS_DEBUG) {
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val) noexcept(!IS_DEBUG) {
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();
			VertexLayout::Bridge<AttributeSetting>(element.GetType(), this, pAttribute, std::forward<T>(val));
		}
	protected:
		Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG);
		template<typename First, typename ...Rest>
		// enables parameter pack setting of multiple parameters by element index
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG) {
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
		// helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val) noexcept(!IS_DEBUG) {
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value) {
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else {
				assert("Parameter attribute type mismatch" && false);
			}
		}
	private:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	// only to read
	class ConstVertex {
	public:
		ConstVertex(const Vertex& v) noexcept;
		template<VertexLayout::ElementType Type>
		const auto& Attr() const noexcept(!IS_DEBUG) {
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:
		Vertex vertex;
	};

	class VertexBuffer {
	public:
		VertexBuffer(VertexLayout layout, size_t size = 0u) noexcept(!IS_DEBUG);
		const char* GetData() const noexcept(!IS_DEBUG);
		const VertexLayout& GetLayout() const noexcept;
		void Resize(size_t newSize) noexcept(!IS_DEBUG);
		size_t Size() const noexcept(!IS_DEBUG);
		size_t SizeBytes() const noexcept(!IS_DEBUG);
		template<typename ...Params>
		void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG) {
			assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex element");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		Vertex Back() noexcept(!IS_DEBUG);
		Vertex Front() noexcept(!IS_DEBUG);
		Vertex operator[](size_t i) noexcept(!IS_DEBUG);
		ConstVertex Back() const noexcept(!IS_DEBUG);
		ConstVertex Front() const noexcept(!IS_DEBUG);
		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG);
	private:
		std::vector<char> buffer;
		VertexLayout layout;
	};
}
