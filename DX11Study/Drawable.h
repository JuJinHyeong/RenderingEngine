#pragma once
#include "Graphics.h"
#include <memory>
#include <DirectXMath.h>
#include "Technique.h"

namespace Bind {
	class Bindable;
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable {
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	
	void Submit(class FrameCommander& frame) const noexcept(!IS_DEBUG);
	void AddTechnique(Technique tech) noexcept;
	void Bind(Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept(!IS_DEBUG);

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);

protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};