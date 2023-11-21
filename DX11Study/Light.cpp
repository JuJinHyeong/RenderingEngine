#include "Light.h"
#include "DynamicConstant.h"

Light::Light(const std::string& name, const Light::Type lightType)
	:
	lightType(lightType),
	SceneObject2(name)
{
	type = SceneObject2::Type::light;
}
