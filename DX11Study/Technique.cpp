#include "Technique.h"
#include "TechniqueProbe.h"
#include "Drawable.h"

Technique::Technique(size_t channels) 
	:
	channels(channels)
{}

Technique::Technique(std::string name, size_t channels, bool startActive) noexcept
	:
	name(std::move(name)),
	channels(channels),
	active(startActive)
{}

void Technique::Submit(const Drawable& drawable, size_t channels) const noexcept {
	// TODO: why compare like this?
	if (active && ((channels & this->channels) != 0)) {
		for (const auto& step : steps) {
			step.Submit(drawable);
		}
	}
}

void Technique::AddStep(Step step) noexcept {
	steps.push_back(std::move(step));
}

bool Technique::IsActive() const noexcept {
	return active;
}

void Technique::SetActiveState(bool active_in) noexcept {
	active = active_in;
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& step : steps) {
		step.InitializeParentReferences(parent);
	}
}

void Technique::Accept(TechniqueProbe& probe) {
	probe.SetTechnique(this);
	for (auto& s : steps) {
		s.Accept(probe);
	}
}

const std::string& Technique::GetName() const noexcept {
	return name;
}

void Technique::Link(Rgph::RenderGraph& rg) {
	for (auto& step : steps) {
		step.Link(rg);
	}
}
