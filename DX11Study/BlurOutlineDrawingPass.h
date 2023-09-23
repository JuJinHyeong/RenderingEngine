#pragma once
#include "RenderQueuePass.h"

class Graphics;

namespace Rgph {
	class BlurOutlineDrawingPass : public RenderQueuePass {
	public:
		BlurOutlineDrawingPass(Graphics& gfx, std::string name, unsigned int fullWidth, unsigned int fullHeight);
		void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) override;
	};
}