#include "XPlanProcess.h"
#include "XRawImage.h"
#include "XPath.h"
#include "XStringTools.h"
#include "XImage.h"

#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;


char* XPlanProcess::image_window = "Source Image";
char* XPlanProcess::result_window = "Result window";
int XPlanProcess::max_Trackbar = 5;
Mat XPlanProcess::img;
Mat XPlanProcess::templ;
Mat XPlanProcess::result;
int XPlanProcess::match_method;
cv::Point2d XPlanProcess::matchloctrouve;




XPlanProcess::XPlanProcess()
{
}


XPlanProcess::~XPlanProcess()
{
}


std::vector<float> XPlanProcess::determinationEquationPlan(std::vector<std::vector<float>> points)
{
	points.size();
	int a = floor(points.size() / 3);
	int b = points.size() - 2 * a;
	
	float sum_x = 0;
	float sum_y = 0;
	float sum_z = 0;

	for (uint32 i = 0; i < a; i++)
	{
		float x = points[i][0];
		float y = points[i][1];
		float z = points[i][2];
		sum_x += x;
		sum_y += y;
		sum_z += z;
	}
	float Ax = sum_x / a;
	float Ay = sum_y / a;
	float Az = sum_z / a;

	
	A << Ax, Ay, Az;

	sum_x = 0;
	sum_y = 0;
	sum_z = 0;
	for (uint32 i = a; i < 2 * a; i++)
	{
		float x = points[i][0];
		float y = points[i][1];
		float z = points[i][2];
		sum_x += x;
		sum_y += y;
		sum_z += z;
	}
	float Bx = sum_x / a;
	float By = sum_y / a;
	float Bz = sum_z / a;

	sum_x = 0;
	sum_y = 0;
	sum_z = 0;
	for (uint32 i = 2 * a; i < points.size(); i++)
	{
		float x = points[i][0];
		float y = points[i][1];
		float z = points[i][2];
		sum_x += x;
		sum_y += y;
		sum_z += z;
	}
	float Cx = sum_x / (points.size() - 2 * a);
	float Cy = sum_y / (points.size() - 2 * a);
	float Cz = sum_z / (points.size() - 2 * a);


	// Vecteurs du plan
	std::vector<float> AB;
	AB.push_back(Bx - Ax);
	AB.push_back(By - Ay);
	AB.push_back(Bz - Az);
	vecteurDirecteur1 = AB;

	std::vector<float> AC;
	AC.push_back(Cx - Ax);
	AC.push_back(Cy - Ay);
	AC.push_back(Cz - Az);



	/* Calcul du produit vectoriel entre AB et AC
		|AB[0]			|AC[0]		|AB[1]*AC[2]-AB[2]*AC[1]
	AB	|AB[1]  ^  AC	|AC[1]	=	|-AB[0]*AC[2] + AB[2]*AC[0]
		|AB[2]			|AC[2]		|AB[0]*AC[1] - AB[1]*AC[0]

	*/
	std::vector<float> normal;
	normal.push_back(AB[1] * AC[2] - AB[2] * AC[1]);
	normal.push_back(-AB[0] * AC[2] + AB[2] * AC[0]);
	normal.push_back(AB[0] * AC[1] - AB[1] * AC[0]);

	
	float sum = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	normal[0] /= sum;
	normal[1] /= sum;
	normal[2] /= sum;
	
	vecteurNormal = normal;

	float d = normal[0] * Ax + normal[1] * Ay + normal[2] * Az;
	std::vector<float> parametresPlan;
	parametresPlan.push_back(normal[0]);
	parametresPlan.push_back(normal[1]);
	parametresPlan.push_back(normal[2]);
	parametresPlan.push_back(d);


	return parametresPlan;

}




float XPlanProcess::profondeurAuPlan(std::vector<float> normal, std::vector<float> A, std::vector<float> M)
{
	// A : point du plan
	// n : normale au plan
	// M : point quelconque à projeter
	std::vector<float> AM;
	AM.push_back(M[0] - A[0]);
	AM.push_back(M[1] - A[1]);
	AM.push_back(M[2] - A[2]);

	/*Produit scalaire*/
	float profondeur = produitScalaire(AM, normal) / norme(normal);
	return profondeur;

}

float XPlanProcess::produitScalaire(std::vector<float> U, std::vector<float> V)
{
	return  U[0] * V[0] + U[1] * V[1] + U[2] * V[2];
}

float XPlanProcess::norme(std::vector<float> U)
{
	return  sqrt(U[0] * U[0] + U[1] * U[1] + U[2] * U[2]);
}

std::vector<float> XPlanProcess::produitVectoriel(std::vector<float> AB, std::vector<float> AC)
{
	std::vector<float> normal;
	normal.push_back(AB[1] * AC[2] - AB[2] * AC[1]);
	normal.push_back(-AB[0] * AC[2] + AB[2] * AC[0]);
	normal.push_back(AB[0] * AC[1] - AB[1] * AC[0]);
	return normal;
}

std::vector<float> XPlanProcess::projectionAuPlan(std::vector<float> parametrePlan, std::vector<float> pointM)
{
	float t = -(parametrePlan[0] * pointM[0] + parametrePlan[1] * pointM[1] + parametrePlan[2] * pointM[2] - parametrePlan[3]) / (parametrePlan[0] * parametrePlan[0] + parametrePlan[1] * parametrePlan[1] + parametrePlan[2] * parametrePlan[2]);

	std::vector<float> MPlan;
	MPlan.push_back(pointM[0] + parametrePlan[0] * t);
	MPlan.push_back(pointM[1] + parametrePlan[1] * t);
	MPlan.push_back(pointM[2] + parametrePlan[2] * t);

	return MPlan;

}

void XPlanProcess::constructionBase()
{
	for (uint32 i = 0; i < vecteurDirecteur1.size(); i++)
	{
		baseXprime.push_back(0);
		baseXprime[i] = vecteurDirecteur1[i];// / norme(vecteurDirecteur1);
	}

	for (uint32 i = 0; i < vecteurNormal.size(); i++)
	{
		baseZprime.push_back(0);
		baseZprime[i] = vecteurNormal[i];// / norme(vecteurNormal);
	}
	
	baseYprime = produitVectoriel(baseZprime, baseXprime);

	float a = norme(baseXprime);
	float b = norme(baseYprime);
	float c = norme(baseZprime);

}

void XPlanProcess::matricePassageReferentielLaserVersReferentielOrthoimage()
{
	P(0, 0) = baseXprime[0];
	P(1, 0) = baseXprime[1];
	P(2, 0) = baseXprime[2];

	P(0, 1) = baseYprime[0];
	P(1, 1) = baseYprime[1];
	P(2, 1) = baseYprime[2];

	P(0, 2) = baseZprime[0];
	P(1, 2) = baseZprime[1];
	P(2, 2) = baseZprime[2];
	std::cout << P << std::endl;
	float a = 0;
}

void XPlanProcess::matricePassageReferentielOrthoimageVersReferentielLaser()
{
	Pinverse = P.inverse();
	std::cout << P.inverse() << std::endl;
	float a = 0;

}


std::vector<std::vector<float>> XPlanProcess::changementEnBaseOrtho(std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesTerrain)
{
	std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho;
	Eigen::Vector3f X(3, 1);
	Eigen::Vector3f Xprime(3, 1);
	std::vector<float> pointTraite;
	
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesTerrain.size(); i++)
	{
		pointTraite = pointsProjetesSurPlanEnCoordonneesTerrain[i];
		X(0, 0) = pointTraite[0]-A(0);
		X(1, 0) = pointTraite[1]-A(1);
		X(2, 0) = pointTraite[2]-A(2);
		Xprime = Pinverse * X;
		std::vector<float> pointTraiteCoordonneesOrtho;
		pointTraiteCoordonneesOrtho.push_back(Xprime(0, 0));
		pointTraiteCoordonneesOrtho.push_back(Xprime(1, 0));
		pointTraiteCoordonneesOrtho.push_back(Xprime(2, 0));

		pointsProjetesSurPlanEnCoordonneesOrtho.push_back(pointTraiteCoordonneesOrtho);
	}
	
	return pointsProjetesSurPlanEnCoordonneesOrtho;
}

std::vector<float> XPlanProcess::bornesDeLOrtho(std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho){
	std::vector<float> XMinYMinXMaxYMax;
	float Xmin;
	float Ymin;
	float Xmax;
	float Ymax;
	Xmin = pointsProjetesSurPlanEnCoordonneesOrtho[0][0];
	Ymin = pointsProjetesSurPlanEnCoordonneesOrtho[0][1]; 
	Xmax = pointsProjetesSurPlanEnCoordonneesOrtho[0][0];
	Ymax = pointsProjetesSurPlanEnCoordonneesOrtho[0][1];

	// Recherche de 
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesOrtho.size(); i++)
	{
		if (pointsProjetesSurPlanEnCoordonneesOrtho[i][0] <= Xmin)
		{
			Xmin = pointsProjetesSurPlanEnCoordonneesOrtho[i][0];
		}
		if (pointsProjetesSurPlanEnCoordonneesOrtho[i][1] <= Ymin)
		{
			Ymin = pointsProjetesSurPlanEnCoordonneesOrtho[i][1];
		}
		if (pointsProjetesSurPlanEnCoordonneesOrtho[i][0] >= Xmax)
		{
			Xmax = pointsProjetesSurPlanEnCoordonneesOrtho[i][0];
		}
		if (pointsProjetesSurPlanEnCoordonneesOrtho[i][1] >= Ymax)
		{
			Ymax = pointsProjetesSurPlanEnCoordonneesOrtho[i][1];
		}
	}

	XMinYMinXMaxYMax.push_back(Xmin);
	XMinYMinXMaxYMax.push_back(Ymin);
	XMinYMinXMaxYMax.push_back(Xmax);
	XMinYMinXMaxYMax.push_back(Ymax);
	
	return XMinYMinXMaxYMax;
}

std::vector<std::vector<float>> XPlanProcess::changementOrigine(std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho, std::vector<float> bornes){
	std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrthoAOrigine;
	std::vector<float> point;
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesOrtho.size(); i++){
		std::vector<float> C;
		point = pointsProjetesSurPlanEnCoordonneesOrtho[i];
		C.push_back(point[0] - bornes[0]);
		C.push_back(point[1] - bornes[1]);
		pointsProjetesSurPlanEnCoordonneesOrthoAOrigine.push_back(C);
	}
	return pointsProjetesSurPlanEnCoordonneesOrthoAOrigine;
}

int XPlanProcess::cherchePixelCompatible(std::vector<std::vector<float>> & pointsProjetesSurPlanEnCoordonneesOrthoTranslate, int colonneEnCours, int ligneEnCours, float taillePixelColonne, float taillePixelLigne)
{
	int indice = -1;
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesOrthoTranslate.size(); i++){
		std::vector<float> point = pointsProjetesSurPlanEnCoordonneesOrthoTranslate[i];
		float x = point[0];
		float y = point[1];
		if (x >= (taillePixelColonne * colonneEnCours) && x <= (taillePixelColonne * (colonneEnCours + 1)) && y >= (taillePixelLigne * ligneEnCours) && y <= (taillePixelLigne * (ligneEnCours + 1)))
		{
			indice = i;
			break;
		}
	}

	return indice;
}
int XPlanProcess::detectionCentreCible(std::string imageFileName, std::string referenceFileName)
{
	/// Load image and template
	img = imread(imageFileName, 1);
	templ = imread(referenceFileName, 1);

	/// Create windows
	namedWindow(image_window, CV_WINDOW_AUTOSIZE);
	namedWindow(result_window, CV_WINDOW_AUTOSIZE);

	/// Create Trackbar
	char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

	MatchingMethod(0, 0);

	std::cout << matchloctrouve.x << " " << matchloctrouve.y << std::endl;
	waitKey(0);
	return 0;
}

/**
* @function MatchingMethod
* @brief Trackbar callback
*/
void XPlanProcess::MatchingMethod(int, void*)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	/// Show me what you got
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow(image_window, img_display);
	imshow(result_window, result);

	

	matchloctrouve = cv::Point2d(matchLoc.x + templ.cols / 2, matchLoc.y + templ.rows / 2);
	return;
}


int XPlanProcess::chercheValeurPixel(const char * filename, int num_ligne, int num_col)
{
	float val = 0;
	int X = 0;
	int Y = 0;
	Mat image;
	image = imread(filename, IMREAD_GRAYSCALE);
	Scalar intensity;
	intensity = image.at<uchar>(num_ligne, num_col);
	std::cout << " TEST " << std::endl;
	std::cout << intensity << std::endl;
	

	//val = pointsProjetesSurPlanEnCoordonneesOrthoTranslate[num_ligne, num_col];
	if (val != -1)
	{
		return val;
	}
	else
	{
		for (X = 0; X < 5; X++)
		{
			for (Y = 0; Y < 5; Y++)
			{
				while (val != -1)
				{
					val = 0;// pointsProjetesSurPlanEnCoordonneesOrthoTranslate[num_ligne + X, num_col + Y];
				}
			}
		}
		return val;
	}
}