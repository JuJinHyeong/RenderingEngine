#pragma once
#include <array>
#include "Job.h"
#include "Pass.h"
#include "Graphics.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BindableCommon.h"

class FrameCommander {
public:
	FrameCommander(Graphics& gfx);
	void Accept(Job job, size_t target) noexcept;
	void Execute(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;
private:
	std::array<Pass, 3> passes;
	DepthStencil ds;
	RenderTarget rt;
	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::PixelShader> pPsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
};