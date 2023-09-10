#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void TestDynamicMeshLoading() {
	using namespace custom;
	Assimp::Importer imp;

	const auto pScene = imp.ReadFile("Models/brick_wall/brick_wall.obj",
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	auto layout = VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Tangent)
		.Append(VertexLayout::Bitangent)
		.Append(VertexLayout::Texture2D);
	VertexBuffer buf{ std::move(layout), *pScene->mMeshes[0] };

	for (auto i = 0ull, end = buf.Size(); i < end; i++) {
		const auto a = buf[i].Attr<VertexLayout::Position3D>();
		const auto b = buf[i].Attr<VertexLayout::Normal>();
		const auto c = buf[i].Attr<VertexLayout::Tangent>();
		const auto d = buf[i].Attr<VertexLayout::Bitangent>();
		const auto e = buf[i].Attr<VertexLayout::Texture2D>();
	}
}