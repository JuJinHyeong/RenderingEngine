#include "FrameCommander.h"
#include "BindableCommon.h"
#include "NullPixelShader.h"

void FrameCommander::Accept(Job job, size_t target) noexcept {
	passes[target].Accept(job);
}

void FrameCommander::Execute(Graphics& gfx) const noexcept(!IS_DEBUG) {
	using namespace Bind;

	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	passes[0].Execute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	passes[1].Execute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	struct SolidColorBuffer {
		DirectX::XMFLOAT4 color = { 1.0f, 0.4f, 0.4f, 1.0f };
	} scb;
	PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, scb, 1u)->Bind(gfx);
	passes[2].Execute(gfx);
}

void FrameCommander::Reset() noexcept {
	for (auto& pass : passes) {
		pass.Reset();
	}
}
