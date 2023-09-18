#include "FrameCommander.h"
#include "BindableCommon.h"
#include "NullPixelShader.h"

FrameCommander::FrameCommander(Graphics& gfx)
	:
	ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
	rt1(gfx, gfx.GetWidth(), gfx.GetHeight()),
	rt2(gfx, gfx.GetWidth(), gfx.GetHeight()),
	blur(gfx, 7, 2.6f, "BlurOutlinePS.cso")
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
	pVsFull = Bind::VertexShader::Resolve(gfx, "FullscreenVS.cso");
	pLayoutFull = Bind::InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());
	pSamplerFull = Bind::Sampler::Resolve(gfx, false, true);
	pBlenderFull = Bind::Blender::Resolve(gfx, true);
}

void FrameCommander::Accept(Job job, size_t target) noexcept {
	passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) noexcept(!IS_DEBUG) {
	using namespace Bind;

	// setup render target used for normal passes
	ds.Clear(gfx);
	rt1.Clear(gfx);
	gfx.BindSwapBuffer(ds);

	// render phong
	Blender::Resolve(gfx, false)->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	passes[0].Execute(gfx);

	// masking
	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	passes[1].Execute(gfx);

	// outline drawing
	rt1.BindAsTarget(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	passes[2].Execute(gfx);
	
	// passes[2] excueted result is stored in rt
	// fullscreen blur + blend pass
	rt2.BindAsTarget(gfx);
	rt1.BindAsTexture(gfx, 0);
	pVbFull->Bind(gfx);
	pIbFull->Bind(gfx);
	pVsFull->Bind(gfx);
	pLayoutFull->Bind(gfx);
	pSamplerFull->Bind(gfx);
	blur.Bind(gfx);
	blur.SetHorizontal(gfx);
	gfx.DrawIndexed(pIbFull->GetCount());
	// fullscreen blur v-pass + combine
	gfx.BindSwapBuffer(ds);
	rt2.BindAsTexture(gfx, 0u);
	pBlenderFull->Bind(gfx);
	pSamplerFull->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	blur.SetVertical(gfx);
	gfx.DrawIndexed(pIbFull->GetCount());
}

void FrameCommander::Reset() noexcept {
	for (auto& pass : passes) {
		pass.Reset();
	}
}

void FrameCommander::ShowWindow(Graphics& gfx)
{
	blur.RenderWidgets(gfx);
}
