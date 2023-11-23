#pragma once
#include <vector>
#include <memory>
#include "SceneObject2.h"

class Camera;
class Graphics;
namespace Rgph {
	class RenderGraph;
}

class CameraContainer : public SceneObject2 {
public:
	CameraContainer();
	void SpawnWindow(Graphics& gfx);
	void Bind(Graphics& gfx);
	void AddCamera(std::shared_ptr<Camera> pCam);
	Camera* operator->();
	~CameraContainer();

	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel) const noexcept(!IS_DEBUG) override;
	Camera& GetActiveCamera();
private:
	Camera& GetControlledCamera();
private:
	std::vector<std::shared_ptr<Camera>> cameras;
	int actived = 0;
	int controlled = 0;
};
