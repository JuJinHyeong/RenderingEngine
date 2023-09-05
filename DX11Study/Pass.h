#pragma once
#include <vector>
#include "Job.h"
#include "Graphics.h"

class Pass {
public:
	void Accept(Job job) noexcept;
	void Execute(Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;
private:
	std::vector<Job> jobs;
};
