#pragma once
#include <string>
#include <fstream>
#include "Timer.h"

class PerformanceLog {
private:
	struct Entry {
		Entry(std::string s, float t)
			:
			label(std::move(s)),
			time(t)
		{}
		void WriteTo(std::ostream& out) const noexcept {
			if (label.empty()) {
				out << time * 1000.0f << "ms\n";
			}
			else {
				out << "[" << label << "] " << time * 1000.0f << "ms\n";
			}
		}
		std::string label;
		float time;
	};

	static PerformanceLog& Get_() noexcept {
		static PerformanceLog log;
		return log;
	}
	PerformanceLog() noexcept;
	~PerformanceLog();
	void Start_(const std::string& label = "") noexcept;
	void Mark_(const std::string& label = "") noexcept;
	void Flush_();
public:
	static void Start(const std::string& label = "") noexcept;
	static void Mark(const std::string& label = "") noexcept;
	
private:
	Timer timer;
	std::vector<Entry> entries;
};

