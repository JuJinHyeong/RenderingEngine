#include "Vertex.h"
#include <string>

custom::VertexLayout::Element::Element(ElementType type, size_t offset)
	:
	type(type),
	offset(offset) {}

size_t custom::VertexLayout::Element::GetOffsetAfter() const noexcept(!IS_DEBUG) {
	return offset + Size();
}

size_t custom::VertexLayout::Element::GetOffset() const {
	return offset;
}

size_t custom::VertexLayout::Element::Size() const noexcept(!IS_DEBUG) {
	return SizeOf(type);
}

constexpr size_t custom::VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG) {
	switch (type) {
	case Position2D:
		return sizeof(Map<Position2D>::SysType);
	case Position3D:
		return sizeof(Map<Position3D>::SysType);
	case Texture2D:
		return sizeof(Map<Texture2D>::SysType);
	case Normal:
		return sizeof(Map<Normal>::SysType);
	case Tangent:
		return sizeof(Map<Tangent>::SysType);
	case Bitangent:
		return sizeof(Map<Bitangent>::SysType);
	case Float3Color:
		return sizeof(Map<Float3Color>::SysType);
	case Float4Color:
		return sizeof(Map<Float4Color>::SysType);
	case BGRAColor:
		return sizeof(Map<BGRAColor>::SysType);
	}
	assert("Invalid element type" && false);
	return 0u;
}

custom::VertexLayout::ElementType custom::VertexLayout::Element::GetType() const noexcept {
	return type;
}

const char* custom::VertexLayout::Element::GetCode() const noexcept {
	switch (type) {
	case Position2D:
		return Map<Position2D>::code;
	case Position3D:
		return Map<Position3D>::code;
	case Texture2D:
		return Map<Texture2D>::code;
	case Normal:
		return Map<Normal>::code;
	case Tangent:
		return Map<Tangent>::code;
	case Bitangent:
		return Map<Bitangent>::code;
	case Float3Color:
		return Map<Float3Color>::code;
	case Float4Color:
		return Map<Float4Color>::code;
	case BGRAColor:
		return Map<BGRAColor>::code;
	default:
		assert("invalid element type" && false);
		return "invalid";
	}
}

D3D11_INPUT_ELEMENT_DESC custom::VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG) {
	switch (type) {
	case Position2D:
		return GenerateDesc<Position2D>(GetOffset());
	case Position3D:
		return GenerateDesc<Position3D>(GetOffset());
	case Texture2D:
		return GenerateDesc<Texture2D>(GetOffset());
	case Normal:
		return GenerateDesc<Normal>(GetOffset());
	case Tangent:
		return GenerateDesc<Tangent>(GetOffset());
	case Bitangent:
		return GenerateDesc<Bitangent>(GetOffset());
	case Float3Color:
		return GenerateDesc<Float3Color>(GetOffset());
	case Float4Color:
		return GenerateDesc<Float4Color>(GetOffset());
	case BGRAColor:
		return GenerateDesc<BGRAColor>(GetOffset());
	}
	assert("Invalid element type" && false);
	return { "INVALID",0,DXGI_FORMAT_UNKNOWN,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
}

const custom::VertexLayout::Element& custom::VertexLayout::ResolveByIndex(size_t i) const noexcept(!IS_DEBUG) {
	return elements[i];
}

custom::VertexLayout& custom::VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG) {
	elements.emplace_back(type, Size());
	return *this;
}

size_t custom::VertexLayout::Size() const noexcept(!IS_DEBUG) {
	return elements.empty() ? 0u : elements.back().GetOffsetAfter();
}

std::string custom::VertexLayout::GetCode() const noexcept {
	std::string code = "";
	for (const custom::VertexLayout::Element& e : elements) {
		code += std::string(e.GetCode());
	}
	return code;
}

size_t custom::VertexLayout::GetElementCount() const noexcept {
	return elements.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> custom::VertexLayout::GetD3DLayout() const noexcept(!IS_DEBUG) {
	std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
	desc.reserve(GetElementCount());
	for (const auto& e : elements) {
		desc.push_back(e.GetDesc());
	}
	return desc;
}

custom::Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG) 
	:
	pData(pData),
	layout(layout) 
{
	assert(pData != nullptr);
}

custom::ConstVertex::ConstVertex(const Vertex& v) noexcept 
	:
	vertex(v) 
{}

custom::VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noexcept(!IS_DEBUG)
	:
	layout(std::move(layout)) 
{
	Resize(size);
}

const custom::VertexLayout& custom::VertexBuffer::GetLayout() const noexcept {
	return layout;
}

void custom::VertexBuffer::Resize(size_t newSize) noexcept(!IS_DEBUG) {
	const auto size = Size();
	if (size < newSize) {
		buffer.resize(buffer.size() + layout.Size() * (newSize - size));
	}
}

size_t custom::VertexBuffer::Size() const noexcept(!IS_DEBUG) {
	return buffer.size() / layout.Size();
}

const char* custom::VertexBuffer::GetData() const noexcept(!IS_DEBUG) {
	return buffer.data();
}

size_t custom::VertexBuffer::SizeBytes() const noexcept(!IS_DEBUG) {
	return buffer.size();
}

custom::Vertex custom::VertexBuffer::Back() noexcept(!IS_DEBUG) {
	assert(buffer.size() != 0u);
	return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
}

custom::Vertex custom::VertexBuffer::Front() noexcept(!IS_DEBUG) {
	assert(buffer.size() != 0u);
	return Vertex{ buffer.data(),layout };
}

custom::Vertex custom::VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG) {
	assert(i < Size());
	return Vertex{ buffer.data() + layout.Size() * i,layout };
}

custom::ConstVertex custom::VertexBuffer::Back() const noexcept(!IS_DEBUG) {
	assert(buffer.size() != 0u);
	return const_cast<VertexBuffer*>(this)->Back();
}

custom::ConstVertex custom::VertexBuffer::Front() const noexcept(!IS_DEBUG) {
	assert(buffer.size() != 0u);
	return const_cast<VertexBuffer*>(this)->Front();
}

custom::ConstVertex custom::VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG) {
	assert(i < Size());
	return const_cast<VertexBuffer&>(*this)[i];
}