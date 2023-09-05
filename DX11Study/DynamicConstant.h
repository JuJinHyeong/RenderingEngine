#pragma once
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>
#include <optional>

#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Matrix) \
	X(Bool)

namespace Dcb {
	namespace dx = DirectX;
	enum Type {
#define X(el) el,
		LEAF_ELEMENT_TYPES
#undef X
		Struct,
		Array,
		Empty,
	};
	// static map of attributes of each leaf type
	template<Type type>
	struct Map {
		static constexpr bool valid = false;
	};
	template<> struct Map<Float> {
		using SysType = float;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F1";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float2> {
		using SysType = dx::XMFLOAT2;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F2";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float3> {
		using SysType = dx::XMFLOAT3;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F3";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float4> {
		using SysType = dx::XMFLOAT4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "F4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Matrix> {
		using SysType = dx::XMFLOAT4X4;
		static constexpr size_t hlslSize = sizeof(SysType);
		static constexpr const char* code = "M4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Bool> {
		using SysType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr const char* code = "BL";
		static constexpr bool valid = true;
	};

#define X(el) static_assert(Map<el>::valid, "Missing implementation for " #el);
	LEAF_ELEMENT_TYPES
#undef X
		template <typename T>
	struct ReverseMap {
		static constexpr bool valid = false;
	};
#define X(el) \
template<> struct ReverseMap<typename Map<el>::SysType> { \
	static constexpr Type type = el; \
	static constexpr bool valid = true; \
};
	LEAF_ELEMENT_TYPES
#undef X

		// layoutElements instances form a tree that describes the layout of the data buffer
		// supporting nested aggregates of structs and arrays
		class LayoutElement {
		private:
			struct ExtraDataBase {
				virtual ~ExtraDataBase() = default;
			};
			friend class RawLayout;
			friend struct ExtraData;
		public:
			// recursive 
			std::string GetSignature() const noexcept(!IS_DEBUG);

			bool Exists() const noexcept;

			std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const noexcept(!IS_DEBUG);

			LayoutElement& operator[](const std::string& key) noexcept(!IS_DEBUG);
			const LayoutElement& operator[](const std::string& key) const noexcept(!IS_DEBUG);

			// get type of leaf element
			LayoutElement& T() noexcept(!IS_DEBUG);
			const LayoutElement& T() const noexcept(!IS_DEBUG);

			size_t GetOffsetBegin() const noexcept(!IS_DEBUG);
			size_t GetOffsetEnd() const noexcept(!IS_DEBUG);
			size_t GetSizeInBytes() const noexcept(!IS_DEBUG);

			LayoutElement& Add(Type addedType, std::string name) noexcept(!IS_DEBUG);
			template<Type typeAdded>
			LayoutElement& Add(std::string key) noexcept(!IS_DEBUG) {
				return Add(typeAdded, std::move(key));
			}

			LayoutElement& Set(Type addedType, size_t size) noexcept(!IS_DEBUG);
			template<Type typeAdded>
			LayoutElement& Set(size_t size) noexcept(!IS_DEBUG) {
				Set(typeAdded, size);
			}
			// return offset of leaf types for read/write purpose
			template<typename T>
			size_t Resolve() const noexcept(!IS_DEBUG) {
				switch (type) {
#define X(el) case el: assert(typeid(Map<el>::SysType) == typeid(T)); return *offset;
					LEAF_ELEMENT_TYPES
#undef X
				default:
					assert("Tried to resolve non-leaf element" && false);
					return 0u;
				}
			}


		private:
			LayoutElement() noexcept = default;
			LayoutElement(Type typeIn) noexcept(!IS_DEBUG);

			size_t Finalize(size_t offsetIn) noexcept(!IS_DEBUG);

			std::string GetSignatureForStruct() const noexcept(!IS_DEBUG);
			std::string GetSignatureForArray() const noexcept(!IS_DEBUG);

			size_t FinalizeForStruct(size_t offsetIn);
			size_t FinalizeForArray(size_t offsetIn);


			static LayoutElement& GetEmptyElement() noexcept;
			// returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
			static size_t AdvanceToBoundary(size_t offset) noexcept;
			// return true if a memory block crosses a boundary
			static bool CrossesBoundary(size_t offset, size_t size) noexcept;
			// advance an offset to next boundary if block crosses a boundary
			static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
			static bool ValidateSymbolName(const std::string& name) noexcept;
		private:
			std::optional<size_t> offset;
			Type type = Empty;
			std::unique_ptr<ExtraDataBase> pExtraData;
	};


	class Layout {
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noexcept(!IS_DEBUG);
	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};

	class RawLayout : public Layout {
		friend class LayoutCodex;
	public:
		RawLayout() noexcept;
		LayoutElement& operator[](const std::string& key) noexcept(!IS_DEBUG);
		template<Type type>
		LayoutElement& Add(const std::string& key) noexcept(!IS_DEBUG) {
			return pRoot->Add<type>(key);
		}
	private:
		void ClearRoot() noexcept;
		// why deliver root? move root is right naming?
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
	};

	class CookedLayout : public Layout {
		friend class LayoutCodex;
		friend class Buffer;
	public:
		const LayoutElement& operator[](const std::string& key) const noexcept(!IS_DEBUG);
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> RelinquishRoot() noexcept;
	};

	class ConstElementRef {
		friend class Buffer;
		friend class ElementRef;
	public:
		class Ptr {
			friend ConstElementRef;
			friend ElementRef;
		public:
			template<typename T>
			operator const T* () const noexcept(!IS_DEBUG) {
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported type used in ElementRef::Ptr::operator T*");
				return &static_cast<const T&>(*ref);
			}
		private:
			Ptr(const ConstElementRef* ref) noexcept;
			const ConstElementRef* ref;
		};
	public:
		bool Exists() const noexcept;
		ConstElementRef operator[](const std::string& key) const noexcept(!IS_DEBUG);
		ConstElementRef operator[](size_t index) const noexcept(!IS_DEBUG);
		Ptr operator&() const noexcept(!IS_DEBUG);
		template<typename T>
		operator const T& () const noexcept(!IS_DEBUG) {
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported type used in ConstElementRef::operator T&");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Resolve<T>());
		}
	private:
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept;
		const LayoutElement* pLayout;
		size_t offset;
		const char* pBytes;
	};

	class ElementRef {
		friend class Buffer;
	public:
		class Ptr {
			friend ElementRef;
		public:
			template<typename T>
			operator T* () const noexcept(!IS_DEBUG) {
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported type used in ElementRef::Ptr::operator T*");
				return &static_cast<T&>(*ref);
			}
		private:
			Ptr(ElementRef* ref) noexcept;
			ElementRef* ref;
		};
	public:
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		ElementRef operator[](const std::string& key) const noexcept(!IS_DEBUG);
		ElementRef operator[](size_t index) const noexcept(!IS_DEBUG);
		template<typename S>
		bool SetifExists(const S& val) noexcept(!IS_DEBUG) {
			if (Exists()) {
				*this = val;
				return true;
			}
			return false;
		}
		Ptr operator&() const noexcept(!IS_DEBUG);
		template<typename T>
		operator T& () const noexcept(!IS_DEBUG) {
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported type used in ElementRef::operator T&");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
		}

		template<typename T>
		T& operator=(const T& rhs) const noexcept(!IS_DEBUG) {
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported type used in ElementRef::operator T&");
			return static_cast<T*>(*this) = rhs;
		}
	private:
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;
		const LayoutElement* pLayout;
		char* pBytes;
		size_t offset;
	};

	class Buffer {
		Buffer(RawLayout&& lay) noexcept(!IS_DEBUG);
		Buffer(const CookedLayout& lay) noexcept(!IS_DEBUG);
		Buffer(CookedLayout&& lay) noexcept(!IS_DEBUG);
		Buffer(const Buffer&) noexcept;
		Buffer(Buffer&&) noexcept;

		ElementRef operator[](const std::string& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](const std::string& key) const noexcept(!IS_DEBUG);
		const char* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;

		const LayoutElement& GetRootLayoutElement() const noexcept;

		void CopyFrom(const Buffer& src) noexcept(!IS_DEBUG);
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};

}

#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif