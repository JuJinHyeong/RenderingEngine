#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "JsonSerializable.h"
#include "Mesh.h"
#include "SceneProbe2.h"

class TechniqueProbe;
class Graphics;

class SceneObject2 : public JsonSerializable {
public:
	enum Type {
		empty,
		light,
		mesh,
	};
	SceneObject2(const std::string& name, const Type type = Type::empty) noexcept;
	void AddChild(const std::shared_ptr<SceneObject2>& childPtr) noexcept;

	virtual void Submit(size_t channel, const DirectX::FXMMATRIX& accumulatedTransform = DirectX::XMMatrixIdentity()) const noexcept(!IS_DEBUG) = 0;
	json ToJson() const noexcept override;
	virtual void Modify(Graphics& gfx, const json& modifiedObject);

	void Accept(SceneProbe2& probe) noexcept;
	virtual void Accept(TechniqueProbe& probe) noexcept;

	const int GetId() const noexcept;
	const Type GetType() const noexcept;
	const std::string& GetName() const noexcept;
	const DirectX::XMMATRIX& GetLocalTransform() const noexcept;
	const bool IsActived() const noexcept;
	void SetActived(bool actived) noexcept;
	virtual void SetLocalTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
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
	bool actived = true;

	std::vector<std::shared_ptr<SceneObject2>> childPtrs;
	std::vector<std::shared_ptr<Drawable>> meshPtrs;
};
