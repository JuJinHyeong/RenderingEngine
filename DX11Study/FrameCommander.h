#pragma once
#include <array>
#include "Job.h"
#include "Pass.h"
#include "Graphics.h"

class FrameCommander {
public:
	void Accept(Job job, size_t target) noexcept;
	void Execute(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;
private:
	std::array<Pass, 3> passes;
};