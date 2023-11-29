#pragma once
#include "IndexedTriangleList.h"
#include "Object.h"
#include "Graphics.h"
#include "SceneObject2.h"
#include "CubeMesh.h"

template<typename T>
class Geometry : public SceneObject2 {
public:
	Geometry(Graphics& gfx, const std::string& name)
		:
		SceneObject2(name, SceneObject2::Type::mesh) {
		meshPtrs.push_back(std::make_shared<T>(gfx));
	}
	void Submit(size_t channel, const DirectX::FXMMATRIX& accumulatedTransform = DirectX::XMMatrixIdentity()) const noexcept(!IS_DEBUG) override {
		std::dynamic_pointer_cast<T>(meshPtrs[0])->Submit(channel, localTransform * accumulatedTransform);
	}
	void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG) override {
		SceneObject2::SetLocalTransform(transform);
		DirectX::XMVECTOR posV, quatV, scaleV;
		DirectX::XMMatrixDecompose(&scaleV, &quatV, &posV, localTransform);

	}
	json ToJson() const noexcept override {
		json j = SceneObject2::ToJson();
		j["mesh"] = std::dynamic_pointer_cast<T>(meshPtrs[0])->ToJson();
		return j;
	}
	void Modify(Graphics& gfx, const json& modifiedObject) noexcept {
		SceneObject2::Modify(gfx, modifiedObject);
		for (auto& meshPtr : meshPtrs) {
			auto geometryMesh = std::dynamic_pointer_cast<T>(meshPtr);
			if (geometryMesh) {
				geometryMesh->Modify(gfx, modifiedObject["mesh"]);
			}
		}
	}
};
