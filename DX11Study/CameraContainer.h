#pragma once
#include <vector>
#include <memory>

class Camera;
class Graphics;

class CameraContainer {
public:
	void SpawnWindow(Graphics& gfx);
	void Bind(Graphics& gfx);
	void AddCamera(std::unique_ptr<Camera> pCam);
	Camera& GetCurCamera();
	~CameraContainer();
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int selected = 0;
};
