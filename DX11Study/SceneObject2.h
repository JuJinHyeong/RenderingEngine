#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "JsonSerializable.h"
#include "Mesh.h"
#include "SceneProbe2.h"

class SceneObject2 : public JsonSerializable {
public:
	enum Type {
		empty,
		light,
		mesh,
	};
	SceneObject2(const std::string& name) noexcept;
	void AddChild(std::shared_ptr<SceneObject2>& childPtr) noexcept;

	void Submit(size_t channel) const noexcept(!IS_DEBUG);
	void Accept(SceneProbe2& probe) noexcept;

	const int GetId() const noexcept;
	const Type GetType() const noexcept;
	const std::string& GetName() const noexcept;
	const DirectX::XMMATRIX& GetLocalTransform() const noexcept;
	void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
	const std::vector<std::shared_ptr<SceneObject2>>& GetChildren() const noexcept;
	const std::vector<std::shared_ptr<Drawable>>& GetMeshes() const noexcept;
	bool HasChildren() const noexcept;
private:
	static int GenerateId() noexcept;
protected:
	int id;
	Type type;
	std::string name;
	DirectX::XMMATRIX localTransform;

	std::vector<std::shared_ptr<SceneObject2>> childPtrs;
	std::vector<std::shared_ptr<Drawable>> meshPtrs;
};