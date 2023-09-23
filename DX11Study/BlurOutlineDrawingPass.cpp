#include "BlurOutlineDrawingPass.h"
#include "BindableCommon.h"
#include "RenderTarget.h"

Rgph::BlurOutlineDrawingPass::BlurOutlineDrawingPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight) 
	:
	RenderQueuePass(std::move(name)) 
{
	using namespace Bind;
	renderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0);
	AddBind(VertexShader::Resolve(gfx, "SolidVS.cso"));
	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));
	AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
	AddBind(Blender::Resolve(gfx, false));
	RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
}

void Rgph::BlurOutlineDrawingPass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG) {
	renderTarget->Clear(gfx);
	RenderQueuePass::Execute(gfx);
}
