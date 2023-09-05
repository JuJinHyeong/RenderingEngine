#include "Technique.h"

void Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept {
	if (active) {
		for (const auto& step : steps) {
			step.Submit(frame, drawable);
		}
	}
}

void Technique::AddStep(Step step) noexcept {
	steps.push_back(std::move(step));
}

void Technique::Activate() noexcept {
	active = true;
}

void Technique::Deactivate() noexcept {
	active = false;
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& step : steps) {
		step.InitializeParentReferences(parent);
	}
}
