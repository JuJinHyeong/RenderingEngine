#pragma once
#include <vector>
#include <memory>
#include "Object.h"

class Camera;
class Graphics;
namespace Rgph {
	class RenderGraph;
}

class CameraContainer : public Object {
public:
	void SpawnWindow(Graphics& gfx);
	void Bind(Graphics& gfx);
	void AddCamera(std::shared_ptr<Camera> pCam);
	Camera* operator->();
	~CameraContainer();

	void LinkTechniques(Rgph::RenderGraph& rg) override;
	void Submit(size_t channel) noexcept(!IS_DEBUG) override;
	Camera& GetActiveCamera();
private:
	Camera& GetControlledCamera();
private:
	std::vector<std::shared_ptr<Camera>> cameras;
	int actived = 0;
	int controlled = 0;
};
