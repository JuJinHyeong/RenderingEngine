#pragma once
#include <memory>
#include <DirectXMath.h>
#include "Graphics.h"
#include "Technique.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Rgph {
	class RenderGraph;
}

namespace Bind {
	class Bindable;
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Material;

class Drawable {
public:
	Drawable() = default;
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;
	
	void Submit() const noexcept;
	void AddTechnique(Technique tech) noexcept;
	void Bind(Graphics& gfx) const noexcept;
	UINT GetIndexCount() const noexcept(!IS_DEBUG);
	void Accept(TechniqueProbe& probe);

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);

	void LinkTechniques(Rgph::RenderGraph& rg);

protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};