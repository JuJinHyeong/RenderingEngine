#include "Material.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG) {

}

custom::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept {
	using Type = custom::VertexLayout::ElementType;
	custom::VertexBuffer buf{vertexLayout};
	buf.Resize(mesh.mNumVertices);
	if (vertexLayout.Has<Type::Position3D>()) {
		for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
			buf[i].Attr<Type::Position3D>() = {
				mesh.mVertices[i].x,
				mesh.mVertices[i].y,
				mesh.mVertices[i].z
			};
		}
	}
	return buf;
}

std::vector<Technique> Material::GetTechniques() const noexcept {
	return techniques;
}
