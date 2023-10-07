#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Blender.h"
#include "NullPixelShader.h"
#include "Camera.h"

class Graphics;

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera(const Camera& cam) noexcept {
			pShadowCamera = &cam;
		}

		ShadowMappingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			depthStencil = std::make_unique<ShaderInputDepthStencil>(gfx, 3, Bind::DepthStencil::Usage::ShadowDepth);
			AddBind(VertexShader::Resolve(gfx, "SolidVS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(Blender::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<Bind::DepthStencil>::Make("map", depthStencil));
		}

		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override {
			depthStencil->Clear(gfx);
			pShadowCamera->BindToGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}

		void DumpShadowMap(Graphics& gfx, const std::string& path) const {
			depthStencil->ToSurface(gfx).Save(path);
		}
	private:
		const Camera* pShadowCamera = nullptr;
	};
}