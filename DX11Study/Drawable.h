#pragma once
#include "Graphics.h"
#include <memory>
#include <DirectXMath.h>

namespace Bind {
	class Bindable;
	class IndexBuffer;
}

class Drawable {
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
protected:
	template<class T>
	T* QueryBindable() noexcept {
		for (auto& pb : binds) {
			if (auto pt = dynamic_cast<T*>(pb.get())) {
				return pt;
			}
		}
		return nullptr;
	}
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);

private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};