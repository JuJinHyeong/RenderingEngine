#include "FrameCommander.h"
#include "BindableCommon.h"
#include "NullPixelShader.h"

FrameCommander::FrameCommander(Graphics& gfx)
	:
	ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
	rt(gfx, gfx.GetWidth(), gfx.GetHeight())
{
	namespace dx = DirectX;

	// setup fullscreen geometry
	custom::VertexLayout lay;
	lay.Append(custom::VertexLayout::Position2D);
	custom::VertexBuffer bufFull{ lay };
	bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
	bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
	pVbFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
	std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
	pIbFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

	// setup fullscreen shaders
	pPsFull = Bind::PixelShader::Resolve(gfx, "FullscreenEffectPS.cso");
	pVsFull = Bind::VertexShader::Resolve(gfx, "FullscreenVS.cso");
	pLayoutFull = Bind::InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());
}

void FrameCommander::Accept(Job job, size_t target) noexcept {
	passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) const noexcept(!IS_DEBUG) {
	using namespace Bind;

	// setup render target used for normal passes
	ds.Clear(gfx);
	rt.BindAsTarget(gfx, ds);

	// render phong
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	passes[0].Execute(gfx);

	// masking
	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	passes[1].Execute(gfx);

	// outline drawing
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	passes[2].Execute(gfx);

	// fullscreen funky pass
	gfx.BindSwapBuffer();
	rt.BindAsTexture(gfx, 0);
	pVbFull->Bind(gfx);
	pIbFull->Bind(gfx);
	pVsFull->Bind(gfx);
	pPsFull->Bind(gfx);
	pLayoutFull->Bind(gfx);
	gfx.DrawIndexed(pIbFull->GetCount());
}

void FrameCommander::Reset() noexcept {
	for (auto& pass : passes) {
		pass.Reset();
	}
}
