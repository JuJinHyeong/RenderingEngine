#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "JsonSerializable.h"
#include "Mesh.h"

class SceneObject2 : public JsonSerializable {
public:
	SceneObject2(const std::string& name) noexcept;
	void AddChild(std::shared_ptr<SceneObject2>& childPtr) noexcept;

	void Submit(size_t channel, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

	json ToJson() const override;
	const int GetId() const noexcept;
	const std::string& GetName() const noexcept;
	const DirectX::XMMATRIX& GetLocalTransform() const noexcept;
	void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
	bool HasChildren() const noexcept;
private:
	static int GenerateId() noexcept;
protected:
	int id;
	std::string name;
	DirectX::XMMATRIX localTransform;

	std::vector<std::shared_ptr<SceneObject2>> childPtrs;
	std::vector<std::shared_ptr<Mesh>> meshPtrs;
};