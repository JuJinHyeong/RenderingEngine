#define DCB_IMPL_SOURCE
#include "DynamicConstant.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <optional>
#include "LayoutCodex.h"

namespace Dcb {
	struct ExtraData {
		struct Struct : public LayoutElement::ExtraDataBase {
			std::vector<std::pair<std::string, LayoutElement>> layoutElements;
		};
		struct Array : public LayoutElement::ExtraDataBase {
			std::optional<LayoutElement> layoutElement;
			size_t size;
		};
	};
#pragma region LayoutElement
	LayoutElement::LayoutElement(Type type) noexcept(!IS_DEBUG)
		:
		type(type)
	{}
	std::string LayoutElement::GetSignature() const noexcept(!IS_DEBUG) {
		switch (type) {
#define X(el) case el: return Map<el>::code;
			LEAF_ELEMENT_TYPES
#undef X
		case Struct:
			return GetSignatureForStruct();
		case Array:
			return GetSignatureForArray();
		default:
			assert("Bad type in signature generation" && false);
			return "???";
		}
	}
	bool LayoutElement::Exists() const noexcept {
		return false;
	}
	std::pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const noexcept(!IS_DEBUG) {
		const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		return { offset + data.layoutElement->GetSizeInBytes() * index, &*data.layoutElement };
	}
	LayoutElement& LayoutElement::operator[](const std::string& key) noexcept(!IS_DEBUG) {
		assert("Keying into non-struct type" && type == Struct);
		for (auto& mem : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements) {
			if (mem.first == key) {
				return mem.second;
			}
		}
		return GetEmptyElement();
		// return empty element if not found
	}
	const LayoutElement& LayoutElement::operator[](const std::string& key) const noexcept(!IS_DEBUG) {
		return const_cast<LayoutElement&>(*this)[key];
	}
	LayoutElement& LayoutElement::T() noexcept(!IS_DEBUG) {
		assert("Accessing T of non-array" && type == Array);
		return *static_cast<ExtraData::Array&>(*pExtraData).layoutElement;
	}
	const LayoutElement& LayoutElement::T() const noexcept(!IS_DEBUG) {
		return const_cast<LayoutElement&>(*this).T();
	}
	size_t LayoutElement::GetOffsetBegin() const noexcept(!IS_DEBUG) {
		return *offset;
	}

	size_t LayoutElement::GetOffsetEnd() const noexcept(!IS_DEBUG) {
		switch (type) {
#define X(el) case el: return *offset + Map<el>::hlslSize;
			LEAF_ELEMENT_TYPES
#undef X
		case Struct:
			{
				const auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
				return 0u;
			}
		case Array:
			{
				const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
				return *offset + AdvanceToBoundary(data.layoutElement->GetSizeInBytes()) * data.size;
			}
			
		default:
			assert("tryied to get offset of empty or invalid element" && false);
			return 0u;
		}

	}
	size_t LayoutElement::GetSizeInBytes() const noexcept(!IS_DEBUG) {
		return GetOffsetEnd() - GetOffsetBegin();
	}

	LayoutElement& LayoutElement::Add(Type addedType, std::string name) noexcept(!IS_DEBUG) {
		assert("Add to non-struct in layout" && type == Struct);
		assert("invalid symbol name in Struct" && false);
		auto& structData = static_cast<ExtraData::Struct&>(*pExtraData);
		for (auto& mem : structData.layoutElements) {
			if (mem.first == name) {
				assert("Adding duplicate symbol to struct" && false);
			}
		}
		structData.layoutElements.emplace_back(std::move(name), LayoutElement{ addedType });
		return *this;
	}

	LayoutElement& LayoutElement::Set(Type addedType, size_t size) noexcept(!IS_DEBUG) {
		assert("Set on non-array in layout" && type == Array);
		assert(size != 0u);
		auto& arrayData = static_cast<ExtraData::Array&>(*pExtraData);
		arrayData.layoutElement = { addedType };
		arrayData.size = size;
		return *this;
	}
	
	size_t LayoutElement::Finalize(size_t offsetIn) noexcept(!IS_DEBUG) {
		switch (type) {
#define X(el) case el: offset = AdvanceIfCrossesBoundary(offsetIn, Map<el>::hlslSize); return *offset + Map<el>::hlslSize;
			LEAF_ELEMENT_TYPES
#undef X
		case Struct:
			return FinalizeForStruct(offsetIn);
		case Array:
			return FinalizeForArray(offsetIn);
		default:
			assert("Bad type in size computation" && false);
			return 0u;
		}
	}

	std::string LayoutElement::GetSignatureForStruct() const noexcept(!IS_DEBUG) {
		using namespace std::string_literals;
		auto sig = "St{"s;
		for (const auto& el : static_cast<ExtraData::Struct&>(*pExtraData).layoutElements) {
			sig += el.first + ":"s + el.second.GetSignature() + ";"s;
		}
		sig += "}"s;
		return sig;
	}

	std::string LayoutElement::GetSignatureForArray() const noexcept(!IS_DEBUG) {
		using namespace std::string_literals;
		const auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		return "Ar:"s + std::to_string(data.size) + "{"s + data.layoutElement->GetSignature() + "}"s;
	}

	size_t LayoutElement::FinalizeForStruct(size_t offsetIn) {
		auto& data = static_cast<ExtraData::Struct&>(*pExtraData);
		assert(data.layoutElements.size() != 0u);
		offset = AdvanceToBoundary(offsetIn);
		auto offsetNext = *offset;
		for (auto& el : data.layoutElements) {
			offsetNext = el.second.Finalize(offsetNext);
		}
		return offsetNext;
	}

	size_t LayoutElement::FinalizeForArray(size_t offsetIn) {
		auto& data = static_cast<ExtraData::Array&>(*pExtraData);
		assert(data.size != 0u);
		offset = AdvanceToBoundary(offsetIn);
		data.layoutElement->Finalize(*offset);
		return GetOffsetEnd();
	}

	LayoutElement& LayoutElement::GetEmptyElement() noexcept {
		static LayoutElement empty{};
		return empty;
	}
	size_t LayoutElement::AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept {
		return CrossesBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
	}
	bool LayoutElement::CrossesBoundary(size_t offset, size_t size) noexcept {
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16 != 0u) || size > 16u;
	}
	size_t LayoutElement::AdvanceToBoundary(size_t offset) noexcept {
		return offset + (16u - offset % 16u) % 16u;
	}
	bool LayoutElement::ValidateSymbolName(const std::string& name) noexcept {
		// symbols can contain alphanumeric and underscore, must not start with digit
		return !name.empty() && !std::isdigit(name.front()) &&
			std::all_of(name.begin(), name.end(), [](char c) {
			return std::isalnum(c) || c == '_';
				}
		);
	}
#pragma endregion


#pragma region Layout
	size_t Layout::GetSizeInBytes() const noexcept {
		return pRoot->GetSizeInBytes();
	}
	std::string Layout::GetSignature() const noexcept(!IS_DEBUG) {
		return pRoot->GetSignature();
	}
	Layout::Layout(std::shared_ptr<LayoutElement> pRoot) noexcept 
		:
		pRoot(std::move(pRoot))
	{}
#pragma endregion


#pragma region RawLayout
	RawLayout::RawLayout() noexcept 
		:
		Layout(std::shared_ptr<LayoutElement>{ new LayoutElement(Struct) })
	{}
	LayoutElement& RawLayout::operator[](const std::string& key) noexcept(!IS_DEBUG) {
		return (*pRoot)[key];
	}
	void RawLayout::ClearRoot() noexcept {
		*this = RawLayout();
	}
	std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept {
		auto temp = std::move(pRoot);
		temp->Finalize(0);
		*this = RawLayout();
		return std::move(temp);
	}
#pragma endregion


#pragma region CookedLayout
	CookedLayout::CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept
		:
		Layout(std::move(pRoot)) 
	{}
	const LayoutElement& CookedLayout::operator[](const std::string& key) const noexcept(!IS_DEBUG) {
		return (*pRoot)[key];
	}
	std::shared_ptr<LayoutElement> CookedLayout::RelinquishRoot() noexcept {
		return std::move(pRoot);
	}
	std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept {
		return pRoot;
	}
#pragma endregion

#pragma region ConstElementRef
	ConstElementRef::Ptr::Ptr(const ConstElementRef* ref) noexcept 
		: ref(ref)
	{}
	bool ConstElementRef::Exists() const noexcept {
		return pLayout->Exists();
	}
	ConstElementRef ConstElementRef::operator[](const std::string& key) const noexcept(!IS_DEBUG) {
		return { &(*pLayout)[key],pBytes,offset };
	}
	ConstElementRef ConstElementRef::operator[](size_t index) const noexcept(!IS_DEBUG) {
		const auto indexingData = pLayout->CalculateIndexingOffset(offset, index);
		return { indexingData.second,pBytes,indexingData.first };
	}
	ConstElementRef::Ptr ConstElementRef::operator&() const noexcept(!IS_DEBUG) {
		return Ptr{ this };
	}
	ConstElementRef::ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept
		:
		offset(offset),
		pLayout(pLayout),
		pBytes(pBytes) 
	{}
#pragma endregion


#pragma region ElementRef
	ElementRef::operator ConstElementRef() const noexcept {
		return { pLayout,pBytes,offset };
	}
	bool ElementRef::Exists() const noexcept {
		return pLayout->Exists();
	}
	ElementRef ElementRef::operator[](const std::string& key) const noexcept(!IS_DEBUG) {
		return { &(*pLayout)[key],pBytes,offset };
	}
	ElementRef ElementRef::operator[](size_t index) const noexcept(!IS_DEBUG) {
		const auto indexingData = pLayout->CalculateIndexingOffset(offset, index);
		return { indexingData.second,pBytes,indexingData.first };
	}
	ElementRef::Ptr ElementRef::operator&() const noexcept(!IS_DEBUG) {
		return Ptr{ const_cast<ElementRef*>(this) };
	}
	ElementRef::ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept
		:
		offset(offset),
		pLayout(pLayout),
		pBytes(pBytes) 
	{}

	ElementRef::Ptr::Ptr(ElementRef* ref) noexcept 
		: 
		ref(ref) 
	{}
#pragma endregion

#pragma region Buffer
	Buffer::Buffer(RawLayout&& lay) noexcept(!IS_DEBUG)
		:
		Buffer(LayoutCodex::Resolve(std::move(lay))) 
	{}
	Buffer::Buffer(const CookedLayout& lay) noexcept(!IS_DEBUG)
		:
		pLayoutRoot(lay.ShareRoot()),
		bytes(pLayoutRoot->GetOffsetEnd()) 
	{}
	Buffer::Buffer(CookedLayout&& lay) noexcept(!IS_DEBUG)
		:
		pLayoutRoot(lay.RelinquishRoot()),
		bytes(pLayoutRoot->GetOffsetEnd()) 
	{}
	Buffer::Buffer(const Buffer& buf) noexcept
		:
		pLayoutRoot(buf.pLayoutRoot),
		bytes(buf.bytes) 
	{}
	Buffer::Buffer(Buffer&& buf) noexcept
		:
		pLayoutRoot(std::move(buf.pLayoutRoot)),
		bytes(std::move(buf.bytes)) 
	{}
	ElementRef Buffer::operator[](const std::string& key) noexcept(!IS_DEBUG) {
		return { &(*pLayoutRoot)[key],bytes.data(),0u };
	}
	ConstElementRef Buffer::operator[](const std::string& key) const noexcept(!IS_DEBUG) {
		return const_cast<Buffer&>(*this)[key];
	}
	const char* Buffer::GetData() const noexcept {
		return bytes.data();
	}
	size_t Buffer::GetSizeInBytes() const noexcept {
		return bytes.size();
	}
	const LayoutElement& Buffer::GetRootLayoutElement() const noexcept {
		return *pLayoutRoot;
	}
	void Buffer::CopyFrom(const Buffer& other) noexcept(!IS_DEBUG) {
		assert(&GetRootLayoutElement() == &other.GetRootLayoutElement());
		std::copy(other.bytes.begin(), other.bytes.end(), bytes.begin());
	}
	std::shared_ptr<LayoutElement> Buffer::ShareLayoutRoot() const noexcept {
		return pLayoutRoot;
	}
#pragma endregion
}