#pragma once
#include "BaseMesh.h"
#include "JsonSerializable.h"
#include "IndexedTriangleList.h"

class CubeMesh : public BaseMesh {
public:
	CubeMesh(Graphics& gfx);
	json ToJson() const noexcept override;
	void SetMaterial(Graphics& gfx, const Material& mat) noexcept;
	const Material& GetMaterial() const noexcept;
	void Modify(Graphics& gfx, const json& modifiedMesh) noexcept;
private:
	std::unique_ptr<IndexedTriangleList> pModel;
	Material mat;
};