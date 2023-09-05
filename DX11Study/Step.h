#pragma once
#include "Bindable.h"

class Step {
public:
	Step(size_t targetPass) noexcept;
	void AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept;
	void Bind(Graphics& gfx) const;
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const;
	void InitializeParentReferences(const class Drawable& parent) noexcept;
private:
	size_t targetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> bindables;
};
