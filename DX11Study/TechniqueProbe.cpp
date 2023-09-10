#include "TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech_in) noexcept {
	pTech = pTech_in;
	techIdx++;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep) noexcept {
	this->pStep = pStep;
	stepIdx++;
	OnSetStep();
}

bool TechniqueProbe::VisitBuffer(Dcb::Buffer& buf) {
	bufIdx++;
	return OnVisitBuffer(buf);
}
