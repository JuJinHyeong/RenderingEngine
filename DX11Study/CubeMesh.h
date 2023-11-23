#pragma once
#include "BaseMesh.h"
#include "JsonSerializable.h"

class CubeMesh : public BaseMesh {
public:
	CubeMesh(Graphics& gfx, float size);
	json ToJson() const noexcept override;
	const Material& GetMaterial() const noexcept;
private:
	Material mat;
};