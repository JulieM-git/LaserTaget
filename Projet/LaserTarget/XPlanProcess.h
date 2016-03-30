#pragma once
#include "XError.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XFileLaser.h"
#include <map>
#include <Eigen/Dense>

class XPlanProcess
{
public:
	XPlanProcess();
	~XPlanProcess();

	std::vector<float> vecteurDirecteur1;
	std::vector<float> vecteurNormal;

	std::vector<float> baseXprime;
	std::vector<float> baseYprime;
	std::vector<float> baseZprime;
	Eigen::Matrix3f P;
	Eigen::Matrix3f Pinverse;
	Eigen::Vector3f A;

	std::vector<float> XPlanProcess::determinationEquationPlan(std::vector<std::vector<float>>);
	float XPlanProcess::profondeurAuPlan(std::vector<float>, std::vector<float>, std::vector<float>);
	float XPlanProcess::produitScalaire(std::vector<float>, std::vector<float>);
	float XPlanProcess::norme(std::vector<float>);
	std::vector<float> XPlanProcess::projectionAuPlan(std::vector<float>, std::vector<float>);
	void XPlanProcess::constructionBase();
	std::vector<float> XPlanProcess::produitVectoriel(std::vector<float>, std::vector<float>);
	void XPlanProcess::matricePassageReferentielLaserVersReferentielOrthoimage();
	void XPlanProcess::matricePassageReferentielOrthoimageVersReferentielLaser();
	std::vector<std::vector<float>> XPlanProcess::changementEnBaseOrtho(std::vector<std::vector<float>>);
	std::vector<float> XPlanProcess::bornesDeLOrtho(std::vector<std::vector<float>>);
};

