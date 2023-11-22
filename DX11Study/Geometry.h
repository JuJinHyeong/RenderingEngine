#pragma once
#include "IndexedTriangleList.h"
#include "Object.h"
#include "Graphics.h"
#include "SceneObject2.h"

template<typename T>
class Geometry : public SceneObject2 {
public:
	Geometry(Graphics& gfx, float size = 1.0f) 
		:
		mesh(gfx, size)
	{}
	void LinkTechniques(Rgph::RenderGraph& rg) {
		mesh.LinkTechniques(rg);
	}
	void Submit(size_t channel) noexcept(!IS_DEBUG) override {
		mesh.Submit(channel, transform);
	}

private:
	T mesh;
};
