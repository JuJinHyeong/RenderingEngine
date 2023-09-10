#pragma once
#include <limits>

namespace Dcb {
	class Buffer;
}

class TechniqueProbe {
public:
	void SetTechnique(class  Technique* pTech) noexcept;
	void SetStep(class Step* pStep) noexcept;
	bool VisitBuffer(Dcb::Buffer& buf);
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
	virtual bool OnVisitBuffer(Dcb::Buffer& buf) = 0;
protected:
	class Technique* pTech = nullptr;
	class Step* pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};
