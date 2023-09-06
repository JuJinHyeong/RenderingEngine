#pragma once
#include <vector>
#include "Step.h"
#include "TechniqueProbe.h"

class Technique {
public:
	Technique() = default;
	Technique(std::string name) noexcept;

	void Submit(class FrameCommander& frame, const class Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool active_in) noexcept;
	// initialize parent when transform c buf needs to be updated
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name = "Nameless Technique";
};
