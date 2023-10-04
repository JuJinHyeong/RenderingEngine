#pragma once
#include <vector>
#include "Step.h"

class TechniqueProbe;
class Drawable;

namespace Rgph {
	class RenderGraph;
}

class Technique {
public:
	Technique(size_t channels);
	Technique(std::string name, size_t channels, bool startActive = true) noexcept;

	void Submit(const Drawable& drawable, size_t channels) const noexcept;
	void AddStep(Step step) noexcept;
	bool IsActive() const noexcept;
	void SetActiveState(bool active_in) noexcept;
	// initialize parent when transform c buf needs to be updated
	void InitializeParentReferences(const class Drawable& parent) noexcept;
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const noexcept;
	void Link(Rgph::RenderGraph& rg);
private:
	bool active = true;
	std::vector<Step> steps;
	std::string name = "Nameless Technique";
	size_t channels;
};
