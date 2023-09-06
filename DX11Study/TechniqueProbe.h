#pragma once

namespace Dcb {
	class Buffer;
}

class TechniqueProbe {
public:
	void SetTechnique(class  Technique* pTech) noexcept;
	void SetStep(class Step* pStep) noexcept;
	virtual bool VisitBuffer(Dcb::Buffer& buf) = 0;
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
protected:
	class Technique* pTech = nullptr;
	class Step* pStep = nullptr;
};
