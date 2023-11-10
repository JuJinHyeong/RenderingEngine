#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>
#include "BindableCommon.h"
#include "BindableCodex.h"
#include "Material.h"

Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept {

}

void Drawable::Submit(size_t channels) const noexcept {
	for (const auto& tech : techniques) {
		tech.Submit(*this, channels);
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

void Drawable::Accept(TechniqueProbe& probe) {
	for (auto& tech : techniques) {
		tech.Accept(probe);
	}
}

void Drawable::AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG) {

}

void Drawable::LinkTechniques(Rgph::RenderGraph& rg) {
	for (auto& tech : techniques) {
		tech.Link(rg);
	}
}
