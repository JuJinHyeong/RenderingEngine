#include "PerformanceLog.h"

PerformanceLog::PerformanceLog() noexcept {
	entries.reserve(3000);
}
PerformanceLog::~PerformanceLog() {
	Flush_();
}

void PerformanceLog::Start_(const std::string& label) noexcept {
	entries.emplace_back(label, 0.0f);
	timer.Mark();
}

void PerformanceLog::Mark_(const std::string& label) noexcept {
	float t = timer.Peek();
	entries.emplace_back(label, t);
}

void PerformanceLog::Flush_() {
	std::ofstream file("Performance.txt");
	file << std::setprecision(3) << std::fixed;
	for (const auto& e : entries) {
		e.WriteTo(file);
	}
}

void PerformanceLog::Start(const std::string& label) noexcept {
	Get_().Start_(label);
}

void PerformanceLog::Mark(const std::string& label) noexcept {
	Get_().Mark_(label);
}
