#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
#include "BindableCommon.h"
#include "BindableCodex.h"

void Drawable::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG) {
	for (const auto& tech : techniques) {
		tech.Submit(frame, *this);
	}
}

void Drawable::AddTechnique(Technique tech) noexcept {
	tech.InitializeParentReferences(*this);
	techniques.push_back(std::move(tech));
}

void Drawable::Bind(Graphics& gfx) const noexcept {
	pVertices->Bind(gfx);
	pIndices->Bind(gfx);
	pTopology->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG) {
	return pIndices->GetCount();
}

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG) {

}
