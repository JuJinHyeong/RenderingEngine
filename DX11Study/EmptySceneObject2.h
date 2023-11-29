#pragma once
#include <string>
#include "SceneObject2.h"

class EmptySceneObject2 : public SceneObject2 {
public:
	EmptySceneObject2(const std::string& name) noexcept;
	void Submit(size_t channel, const DirectX::FXMMATRIX& accumulatedTransform = DirectX::XMMatrixIdentity()) const noexcept(!IS_DEBUG) override;
	json ToJson() const noexcept override;
};