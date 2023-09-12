#include "Vertex.h"
#include <string>

namespace custom {
	VertexLayout::Element::Element(ElementType type, size_t offset)
		:
		type(type),
		offset(offset) {}

	size_t VertexLayout::Element::GetOffsetAfter() const noexcept(!IS_DEBUG) {
		return offset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const {
		return offset;
	}

	size_t VertexLayout::Element::Size() const noexcept(!IS_DEBUG) {
		return SizeOf(type);
	}

	template<VertexLayout::ElementType type>
	struct SysSizeLookup {
		static constexpr auto Exec() noexcept {
			return sizeof(VertexLayout::Map<type>::SysType);
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG) {
		return Bridge<SysSizeLookup>(type);
	}

	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept {
		return type;
	}

	template<VertexLayout::ElementType type>
	struct SysCodeLookup {
		static constexpr auto Exec() noexcept {
			return VertexLayout::Map<type>::code;
		}
	};
	const char* VertexLayout::Element::GetCode() const noexcept {
		return Bridge<SysCodeLookup>(type);
	}

	template<VertexLayout::ElementType type>
	struct DescGenerate {
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept {
			return {
				VertexLayout::Map<type>::semantic,
				0,
				VertexLayout::Map<type>::dxgiFormat,
				0,
				(UINT)offset,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			};
		};
	};
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG) {
		return Bridge<DescGenerate>(type, GetOffset());
	}

	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noexcept(!IS_DEBUG) {
		return elements[i];
	}

	VertexLayout& VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG) {
		if (!Has(type)) {
			elements.emplace_back(type, Size());
		}
		return *this;
	}

	size_t VertexLayout::Size() const noexcept(!IS_DEBUG) {
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}

	std::string VertexLayout::GetCode() const noexcept {
		std::string code = "";
		for (const VertexLayout::Element& e : elements) {
			code += std::string(e.GetCode());
		}
		return code;
	}

	size_t VertexLayout::GetElementCount() const noexcept {
		return elements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noexcept(!IS_DEBUG) {
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements) {
			desc.push_back(e.GetDesc());
		}
		return desc;
	}

	bool VertexLayout::Has(ElementType type) const noexcept {
		for (auto& el : elements) {
			if (el.GetType() == type) {
				return true;
			}
		}
		return false;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
		:
		pData(pData),
		layout(layout) {
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& v) noexcept
		:
		vertex(v) {}

	VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noexcept(!IS_DEBUG)
		:
		layout(std::move(layout)) {
		Resize(size);
	}

	template<VertexLayout::ElementType type>
	struct AttributeAiMeshFill {
		static constexpr void Exec(VertexBuffer* pBuf, const aiMesh& mesh) noexcept(!IS_DEBUG) {
			for (size_t i = 0u, end = mesh.mNumVertices; i < end; i++) {
				(*pBuf)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh, i);
			}
		};
	};
	VertexBuffer::VertexBuffer(VertexLayout layout_in, const aiMesh& mesh) 
		:
		layout(std::move(layout_in))
	{
		Resize(mesh.mNumVertices);
		for (size_t i = 0, end = layout.GetElementCount(); i < end; i++) {
			VertexLayout::Bridge<AttributeAiMeshFill>(layout.ResolveByIndex(i).GetType(), this, mesh);
		}
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept {
		return layout;
	}

	void VertexBuffer::Resize(size_t newSize) noexcept(!IS_DEBUG) {
		const auto size = Size();
		if (size < newSize) {
			buffer.resize(buffer.size() + layout.Size() * (newSize - size));
		}
	}

	size_t VertexBuffer::Size() const noexcept(!IS_DEBUG) {
		return buffer.size() / layout.Size();
	}

	const char* VertexBuffer::GetData() const noexcept(!IS_DEBUG) {
		return buffer.data();
	}

	size_t VertexBuffer::SizeBytes() const noexcept(!IS_DEBUG) {
		return buffer.size();
	}

	Vertex VertexBuffer::Back() noexcept(!IS_DEBUG) {
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}

	Vertex VertexBuffer::Front() noexcept(!IS_DEBUG) {
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(),layout };
	}

	Vertex VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG) {
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}

	ConstVertex VertexBuffer::Back() const noexcept(!IS_DEBUG) {
		assert(buffer.size() != 0u);
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex VertexBuffer::Front() const noexcept(!IS_DEBUG) {
		assert(buffer.size() != 0u);
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG) {
		assert(i < Size());
		return const_cast<VertexBuffer&>(*this)[i];
	}
}