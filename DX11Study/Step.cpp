#include "Step.h"
#include "FrameCommander.h"
#include "Drawable.h"

Step::Step(size_t targetPass) noexcept 
	:
	targetPass(targetPass)
{}

void Step::AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept {
	bindables.push_back(std::move(bind));
}

void Step::Bind(Graphics& gfx) const {
	for (const auto& bind : bindables) {
		bind->Bind(gfx);
	}
}

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const {
	frame.Accept(Job{ this, &drawable }, targetPass);
}

void Step::InitializeParentReferences(const Drawable& parent) noexcept {
	for (auto& bind : bindables) {
		bind->InitializeParentReference(parent);
	}
}

void Step::Accept(TechniqueProbe& probe) {
	probe.SetStep(this);
	for (auto& b : bindables) {
		b->Accept(probe);
	}
}
