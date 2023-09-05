#pragma once
#include <vector>
#include "Step.h"

class Technique {
public:
	void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	void Activate() noexcept;
	void Deactivate() noexcept;
	void InitializeParentReferences(const class Drawable& parent) noexcept;
private:
	bool active = true;
	std::vector<Step> steps;
};
