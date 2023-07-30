#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"

class IndexedTriangleList {
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(custom::VertexBuffer verts_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in)) {
		assert(vertices.Size() > 2);
		assert(indices.size() % 3 == 0);
	}
	void Transform(DirectX::FXMMATRIX matrix) {
		using Elements = custom::VertexLayout::ElementType;
		for (int i = 0; i < vertices.Size(); i++) {
			auto& pos = vertices[i].Attr<Elements::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix)
			);
		}
	}
public:
	custom::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};
