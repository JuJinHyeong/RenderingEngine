#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind {
	VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag, const custom::VertexBuffer& vbuf)
		:
		tag(tag),
		stride((UINT)vbuf.GetLayout().Size()),
		layout(vbuf.GetLayout())
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0u;
		vbd.MiscFlags = 0u;
		vbd.ByteWidth = (UINT)vbuf.SizeBytes();
		vbd.StructureByteStride = stride;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();

		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &sd, &pVertexBuffer));
	}

	VertexBuffer::VertexBuffer(Graphics& gfx, const custom::VertexBuffer& vbuf)
		:
		VertexBuffer(gfx, "not in codex system", vbuf)
	{}

	void VertexBuffer::Bind(Graphics& gfx) noexcept {
		const UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}

	const custom::VertexLayout& VertexBuffer::GetLayout() const noexcept {
		return layout;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag, const custom::VertexBuffer& vbuf) {
		assert(tag != "not in codex system");
		return Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
	}
	std::string VertexBuffer::_GenerateUID(const std::string& tag) {
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
	std::string VertexBuffer::GetUID() const noexcept {
		return _GenerateUID(tag);
	}
}
