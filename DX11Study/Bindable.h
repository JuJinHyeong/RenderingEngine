#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"
#include <string>

class Drawable;
class TechniqueProbe;

namespace Bind {
	class Bindable : public GraphicsResource {
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;

		virtual void InitializeParentReference(const Drawable& parent) noexcept {};

		virtual std::string GetUID() const noexcept;
		virtual ~Bindable() = default;
		// use only when bindable class havs dyanamic constant buffer
		virtual void Accept(TechniqueProbe& probe) {};
	};
}