#pragma once
#include "XError.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XFileLaser.h"
#include <map>

class XPlanProcess
{
public:
	XPlanProcess();
	~XPlanProcess();


	std::vector<float> XPlanProcess::determinationEquationPlan(std::vector<std::vector<float>>);
	float XPlanProcess::profondeurAuPlan(std::vector<float>, std::vector<float>, std::vector<float>);
	float XPlanProcess::produitScalaire(std::vector<float>, std::vector<float>);
	float XPlanProcess::norme(std::vector<float>);
	std::vector<float> XPlanProcess::projectionAuPlan(std::vector<float>, std::vector<float>);
};

