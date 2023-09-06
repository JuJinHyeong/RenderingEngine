#include "TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech_in) noexcept {
	pTech = pTech_in;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep) noexcept {
	this->pStep = pStep;
	OnSetStep();
}
