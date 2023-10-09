#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "ShadowCameraCBuf.h"
#include "DepthStencil.h"
#include "ShadowSampler.h"

class Graphics;

namespace Rgph {
	class LambertianPass : public RenderQueuePass {
	public:
		LambertianPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name)),
			pShadowCBuf{ std::make_shared<Bind::ShadowCameraCBuf>(gfx) }
		{
			using namespace Bind;
			AddBind(pShadowCBuf);
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBindSink<Bind::Bindable>("shadowMap");
			AddBind(std::make_shared<Bind::ShadowSampler>(gfx));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override
		{
			assert(pMainCamera);
			pShadowCBuf->Update(gfx);
			pMainCamera->BindToGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}
		void BindMainCamera(const Camera& cam) noexcept {
			pMainCamera = &cam;
		}
		void BindShadowCamera(const Camera& cam) noexcept {
			pShadowCBuf->SetCamera(&cam);
		}
	private:
		std::shared_ptr<Bind::ShadowCameraCBuf> pShadowCBuf;
		const Camera* pMainCamera = nullptr;
	};
}