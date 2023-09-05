#include "Pass.h"

void Pass::Accept(Job job) noexcept {
	jobs.push_back(std::move(job));
}

void Pass::Execute(Graphics& gfx) const noexcept(!IS_DEBUG) {
	for (const auto& job : jobs) {
		job.Execute(gfx);
	}
}

void Pass::Reset() noexcept {
	jobs.clear();
}
