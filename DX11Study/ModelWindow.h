#pragma once
#include "Node.h"

// ModelWindow class doesn't need to be read from another header.
// so ModelWindow class is defined in cpp file.
class ModelWindow {
public:
	void Show(const char* windowName, const Node& root) noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetTransform() const noexcept(!IS_DEBUG);
	Node* GetSelectedNode() const noexcept(!IS_DEBUG);
private:
	Node* pSelectedNode = nullptr;
	struct TransformParameters {
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};
