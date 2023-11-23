#pragma once
#include "IndexedTriangleList.h"
#include "Object.h"
#include "Graphics.h"
#include "SceneObject2.h"
#include "CubeMesh.h"

template<typename T>
class Geometry : public SceneObject2 {
public:
	Geometry(Graphics& gfx, const std::string& name, float size = 1.0f)
		:
		SceneObject2(name, SceneObject2::Type::mesh) {
		meshPtrs.push_back(std::make_shared<T>(gfx, size));
	}
	void Submit(size_t channel) const noexcept(!IS_DEBUG) override {
		std::dynamic_pointer_cast<T>(meshPtrs[0])->Submit(channel, localTransform);
	}

	json ToJson() const noexcept override {
		json j = SceneObject2::ToJson();
		return j;
	}
};
