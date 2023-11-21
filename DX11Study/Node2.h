#pragma once
#include "SceneObject2.h"
#include <DirectXMath.h>

class Node2 : public SceneObject2 {
public:
	Node2(const aiNode& pNode, const std::vector<std::shared_ptr<Mesh>>& modelMeshPtrs);
	json ToJson() const override;
	void Submit(size_t channel, DirectX::FXMMATRIX acuumulatedTransform) const noexcept(!IS_DEBUG);
private:
	DirectX::XMMATRIX relativeTransform;
};