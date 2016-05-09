#pragma once
#include "XError.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XFileLaser.h"
#include <map>
#include <Eigen/Dense>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <iostream>
#include <stdio.h>


using namespace cv;
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

	static cv::Point2d matchloctrouve;

	std::vector<float> bornes;

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
	std::vector<std::vector<float>> XPlanProcess::changementOrigine(std::vector<std::vector<float>>, std::vector<float>);



	int XPlanProcess::cherchePixelCompatible(std::vector<std::vector<float>> &, int, int, float, float);


	int XPlanProcess::chercheValeurPixel(const char * filename, int num_ligne, int num_col);

	static int XPlanProcess::detectionCentreCible(std::string, std::string);
	cv::Mat XPlanProcess::openImage(std::string);
	cv::Mat XPlanProcess::convertToInt(cv::Mat);
	
	/// Global Variables
	static Mat img;
	static Mat templ;
	static Mat result;
	static char* image_window;
	static char* result_window;
	static int max_Trackbar;
	static int match_method;

	/// Function Headers
	static void MatchingMethod(int, void*);

};

