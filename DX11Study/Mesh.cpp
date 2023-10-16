#include "Mesh.h"
#include "Channels.h"
#include "Vertex.h"

Mesh::Mesh(Graphics& gfx, const Material2& mat, const aiMesh& mesh, float scale) noexcept(!IS_DEBUG)
{
	using namespace Bind;
	custom::VertexLayout vertexLayout;
	{
		Technique phong{ "Phong", channel::main };

		Step step("lambertian");
		std::string shaderCode = "Phong";
		vertexLayout.Append(custom::VertexLayout::Position3D);
		vertexLayout.Append(custom::VertexLayout::Normal);
		
	}
}

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept(!IS_DEBUG)
	:
	Drawable(gfx, mat, mesh, scale)
{}

// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs) {
	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	for (auto& pb : bindPtrs) {
		AddBind(std::move(pb));
	}
	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept {
	return DirectX::XMLoadFloat4x4(&transform);
}

void Mesh::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG) {
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Submit(channels);
}
