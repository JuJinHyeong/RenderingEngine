#pragma once
#include "SceneObject2.h"
#include "ConstantBufferEx.h"

class Light : public SceneObject2 {
public:
	enum Type {
		point,
		directional,
	};
	Light(const std::string& name, const Light::Type type);
protected:
	Light::Type lightType;
	std::unique_ptr<Bind::CachingPixelConstantBufferEx> lightCBuffer;
};