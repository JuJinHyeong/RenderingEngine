#pragma once
#include <array>
#include "Job.h"
#include "Pass.h"
#include "Graphics.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BindableCommon.h"
#include "BlurPack.h"

class FrameCommander {
public:
	FrameCommander(Graphics& gfx);
	void Accept(Job job, size_t target) noexcept;
	void Execute(Graphics& gfx) noexcept(!IS_DEBUG);
	void Reset() noexcept;
	void ShowWindow(Graphics& gfx);
private:
	std::array<Pass, 3> passes;
	int downFactor = 2;
	DepthStencil ds;
	std::optional<RenderTarget> rt1;
	std::optional<RenderTarget> rt2;
	BlurPack blur;
	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::PixelShader> pPsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> pSamplerFullBilin;
	std::shared_ptr<Bind::Blender> pBlenderFull;
};