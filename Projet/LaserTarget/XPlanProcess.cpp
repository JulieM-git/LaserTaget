// Appels des fichiers
#include "XPlanProcess.h"
#include "XRawImage.h"
#include "XPath.h"
#include "XStringTools.h"
#include "XImage.h"

// Chargement des modules
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <iostream>
#include <stdio.h>

// Namespace
using namespace cv;

// Définition des variables statics
char* XPlanProcess::image_window = "Source Image";
char* XPlanProcess::result_window = "Result window";
int XPlanProcess::max_Trackbar = 5;
Mat XPlanProcess::img;
Mat XPlanProcess::templ;
Mat XPlanProcess::result;
int XPlanProcess::match_method=5;
bool XPlanProcess::display = 0;
cv::Point2d XPlanProcess::matchloctrouve;



// Constructeur de la classe
XPlanProcess::XPlanProcess()
{
}

// Destructeur de la classe
XPlanProcess::~XPlanProcess()
{
}

// Fonction qui détermine l'equation d'un plan à partir d'une liste de points. Retourne le vecteur de paramètres de l'equation :
// ax+by+cz=d => [a, b, c, d]
std::vector<float> XPlanProcess::determinationEquationPlan(std::vector<std::vector<float>> points)
{
	///////////////
	////	METHODE INITIALE : NE FIT PAS LE PLAN IDEAL
	//// Divise le nuage en 3 sous-nuages et détermine le barycentre de chaque sous-nuage
	///////////////
	//points.size();
	//int a = floor(points.size() / 3);
	//int b = points.size() - 2 * a;
	//
	//// Premier sous-nuage
	//float sum_x = 0;
	//float sum_y = 0;
	//float sum_z = 0;

	//for (uint32 i = 0; i < a; i++)
	//{
	//	float x = points[i][0];
	//	float y = points[i][1];
	//	float z = points[i][2];
	//	sum_x += x;
	//	sum_y += y;
	//	sum_z += z;
	//}
	//float Ax = sum_x / a;
	//float Ay = sum_y / a;
	//float Az = sum_z / a;

	//
	//A << Ax, Ay, Az;

	//// Deuxieme sous-nuage
	//sum_x = 0;
	//sum_y = 0;
	//sum_z = 0;
	//for (uint32 i = a; i < 2 * a; i++)
	//{
	//	float x = points[i][0];
	//	float y = points[i][1];
	//	float z = points[i][2];
	//	sum_x += x;
	//	sum_y += y;
	//	sum_z += z;
	//}
	//float Bx = sum_x / a;
	//float By = sum_y / a;
	//float Bz = sum_z / a;

	//// Troisieme sous-nuage
	//sum_x = 0;
	//sum_y = 0;
	//sum_z = 0;
	//for (uint32 i = 2 * a; i < points.size(); i++)
	//{
	//	float x = points[i][0];
	//	float y = points[i][1];
	//	float z = points[i][2];
	//	sum_x += x;
	//	sum_y += y;
	//	sum_z += z;
	//}
	//float Cx = sum_x / (points.size() - 2 * a);
	//float Cy = sum_y / (points.size() - 2 * a);
	//float Cz = sum_z / (points.size() - 2 * a);


	//// Définition de vecteurs du plan
	//std::vector<float> AB;
	//AB.push_back(Bx - Ax);
	//AB.push_back(By - Ay);
	//AB.push_back(Bz - Az);
	//vecteurDirecteur1 = AB;

	//std::vector<float> AC;
	//AC.push_back(Cx - Ax);
	//AC.push_back(Cy - Ay);
	//AC.push_back(Cz - Az);



	///* Calcul du produit vectoriel entre AB et AC
	//	|AB[0]			|AC[0]		|AB[1]*AC[2]-AB[2]*AC[1]
	//AB	|AB[1]  ^  AC	|AC[1]	=	|-AB[0]*AC[2] + AB[2]*AC[0]
	//	|AB[2]			|AC[2]		|AB[0]*AC[1] - AB[1]*AC[0]

	//*/
	//std::vector<float> normal;
	//normal.push_back(AB[1] * AC[2] - AB[2] * AC[1]);
	//normal.push_back(-AB[0] * AC[2] + AB[2] * AC[0]);
	//normal.push_back(AB[0] * AC[1] - AB[1] * AC[0]);

	//// Normalisation du vecteur normal
	//float sum = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	//normal[0] /= sum;
	//normal[1] /= sum;
	//normal[2] /= sum;
	//
	//vecteurNormal = normal;

	//float d = normal[0] * Ax + normal[1] * Ay + normal[2] * Az;

	//std::vector<float> parametresPlan;
	//parametresPlan.push_back(normal[0]);
	//parametresPlan.push_back(normal[1]);
	//parametresPlan.push_back(normal[2]);
	//parametresPlan.push_back(d);


	//return parametresPlan;

///////////////////////////
	///////////////
	////	METHODE AVANCEE : FIT UN PLAN IDEAL PAR MOINDRES CARRES
	//// Estime les parametres du plan par moindres carrés
	///////////////

	// Creation des matrices pour les moindres carrés
	// On pose le probleme sous la forme :  ax+by+d = z
	Eigen::Matrix<double, Eigen::Dynamic, 3> modeleA;
	modeleA.resize(points.size(), 3);

	Eigen::Matrix<double, Eigen::Dynamic, 1> B;
	B.resize(points.size(), 1);

	Eigen::MatrixXd P = Eigen::MatrixXd::Identity(points.size(), points.size());
	
	for (int j = 0; j < points.size(); j++)
	{
		modeleA.row(j) << points[j][0], points[j][1], 1;  // Coordonnees homogenes des points en entree
		B.row(j) << points[j][2];
	}

	// Résolution par moindres carrés
	Eigen::MatrixXd N = modeleA.transpose() * P * modeleA;
	Eigen::MatrixXd K = modeleA.transpose() * P * B;

	Eigen::Vector3d X = N.inverse() * K;

	// On ramene le probleme à la forme : alpha x + beta y + gamma z + delta = 0
	float alpha;
	float beta;
	float gamma;
	float delta;

	float a = X(0);
	float b = X(1);
	float d = X(2);
	gamma = sqrt(1 / (a*a + b*b));
	alpha = -a * gamma;
	beta = -b * gamma;
	delta = -d * gamma;

	std::vector<float> parametresPlan;
	parametresPlan.push_back(alpha);
	parametresPlan.push_back(beta);
	parametresPlan.push_back(gamma);
	parametresPlan.push_back(-delta); // Attention, par moindres carrés on obtient les parametres :
	// alpha x + beta y + gamma z + delta = 0 et non : alpha x + beta y + gamma z = delta

	// Définition du vecteur normal
	std::vector<float> normal;
	normal.push_back(alpha);
	normal.push_back(beta);
	normal.push_back(gamma);
	vecteurNormal = normal;

	// ax+by+cz=d
	// point 1 appartenant au plan
	double x1 = 0;
	double z1 = 0;
	double y1 = 0;
	try {
		y1 = (-parametresPlan[0] * x1 - parametresPlan[2] * z1 + parametresPlan[3]) / parametresPlan[1];
	}
	catch (...) {
		std::cout << "Exception occurred : Le parametre b est peut etre nul";
	}
	A << x1, y1, z1;

	// point 2 appartenant au plan
	double x2 = 1;
	double z2 = 0;
	double y2;
	try {
		y2 = (-parametresPlan[0] * x2 - parametresPlan[2] * z2 + parametresPlan[3]) / parametresPlan[1];
	}
	catch (...) {
		std::cout << "Exception occurred : Le parametre b est peut etre nul";
	}

	// un vecteur directeur du plan
	std::vector<float> AB;
	AB.push_back(x2 - x1);
	AB.push_back(y2 - y1);
	AB.push_back(z2 - z1);
	vecteurDirecteur1 = AB;

	return parametresPlan;

}



// Calcul de la distance orthogonale d'un point M par rapport au plan
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

// Calcul du produit scalaire entre deux vecteurs 
float XPlanProcess::produitScalaire(std::vector<float> U, std::vector<float> V)
{
	return  U[0] * V[0] + U[1] * V[1] + U[2] * V[2];
}

// Calcul de la norme d'un vecteur
float XPlanProcess::norme(std::vector<float> U)
{
	return  sqrt(U[0] * U[0] + U[1] * U[1] + U[2] * U[2]);
}

// Calcul du produit vectoriel de deux vecteurs
std::vector<float> XPlanProcess::produitVectoriel(std::vector<float> AB, std::vector<float> AC)
{
	std::vector<float> normal;
	normal.push_back(AB[1] * AC[2] - AB[2] * AC[1]);
	normal.push_back(-AB[0] * AC[2] + AB[2] * AC[0]);
	normal.push_back(AB[0] * AC[1] - AB[1] * AC[0]);
	return normal;
}

// Projete un pointM sur le plan défini par parametrePlan. Retourne un vecteur correspondant aux coordonnées du pointM projeté.
std::vector<float> XPlanProcess::projectionAuPlan(std::vector<float> parametrePlan, std::vector<float> pointM)
{
	float t = -(parametrePlan[0] * pointM[0] + parametrePlan[1] * pointM[1] + parametrePlan[2] * pointM[2] - parametrePlan[3]) / (parametrePlan[0] * parametrePlan[0] + parametrePlan[1] * parametrePlan[1] + parametrePlan[2] * parametrePlan[2]);

	std::vector<float> MPlan;
	MPlan.push_back(pointM[0] + parametrePlan[0] * t);
	MPlan.push_back(pointM[1] + parametrePlan[1] * t);
	MPlan.push_back(pointM[2] + parametrePlan[2] * t);

	return MPlan;

}

// Construit une base orthogonale dont un axe est un vecteur directeur du plan et un second axe est le vecteur normal au plan
void XPlanProcess::constructionBase()
{
	for (uint32 i = 0; i < vecteurDirecteur1.size(); i++)
	{
		baseXprime.push_back(0);
		baseXprime[i] = vecteurDirecteur1[i];
	}

	for (uint32 i = 0; i < vecteurNormal.size(); i++)
	{
		baseZprime.push_back(0);
		baseZprime[i] = vecteurNormal[i];
	}
	
	baseYprime = produitVectoriel(baseZprime, baseXprime);
}

// Construit la matrice de passage entre une base supposée orthonormée du nuage de point et la base orthonormée dont les axes sont : un vecteur directeur
// du plan et le vecteur normal au plan
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
	std::cout << "Matrice de passage du referentiel laser au referentiel de l orthoimage : " << std::endl << P << std::endl << std::endl;
}

// Construit la matrice de passage inverse entre une base supposée orthonormée du nuage de point et la base orthonormée dont les axes sont : un vecteur directeur
// du plan et le vecteur normal au plan
void XPlanProcess::matricePassageReferentielOrthoimageVersReferentielLaser()
{
	Pinverse = P.inverse();
	std::cout << "Matrice de passage inverse du referentiel laser au referentiel de l orthoimage : " << std::endl << P.inverse() << std::endl << std::endl;
}

// Applique un changement de base aux points en coordonnées terrain pour les transformer en coordonnées de la base orthonormée du plan
std::vector<std::vector<float>> XPlanProcess::changementEnBaseOrtho(std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesTerrain)
{
	std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho;
	Eigen::Vector3f X(3, 1);
	Eigen::Vector3f Xprime(3, 1);
	std::vector<float> pointTraite;
	
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesTerrain.size(); i++)
	{
		pointTraite = pointsProjetesSurPlanEnCoordonneesTerrain[i];
		// Translation du point à cause du changement d'origine (A origine de la base orthonormée pour le plan)
		X(0, 0) = pointTraite[0]-A(0);
		X(1, 0) = pointTraite[1]-A(1);
		X(2, 0) = pointTraite[2]-A(2);

		// Changement de base
		Xprime = Pinverse * X;

		// Sauvegarde des coordonnées
		std::vector<float> pointTraiteCoordonneesOrtho;
		pointTraiteCoordonneesOrtho.push_back(Xprime(0, 0));
		pointTraiteCoordonneesOrtho.push_back(Xprime(1, 0));
		pointTraiteCoordonneesOrtho.push_back(Xprime(2, 0));

		pointsProjetesSurPlanEnCoordonneesOrtho.push_back(pointTraiteCoordonneesOrtho);
	}
	
	return pointsProjetesSurPlanEnCoordonneesOrtho;
}


// Détermination des limites de la projection du nuage sur le plan (non finie) pour générer une image tif (finie)
// Retourne un vecteur qui correspond à [XMin, YMin, XMax, ZMax]
std::vector<float> XPlanProcess::bornesDeLOrtho(std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho){
	std::vector<float> XMinYMinXMaxYMax;

	float Xmin = pointsProjetesSurPlanEnCoordonneesOrtho[0][0];
	float Ymin = pointsProjetesSurPlanEnCoordonneesOrtho[0][1];
	float Xmax = pointsProjetesSurPlanEnCoordonneesOrtho[0][0];
	float Ymax = pointsProjetesSurPlanEnCoordonneesOrtho[0][1];

	// Recherche des bornes
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


// Fonction qui translate tous les points d'un nuage de points projeté sur un plan pour la génération de l'orthoimage
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

// Fonction qui retourne l'indice du premier point rencontré dans le nuage qui se trouve dans la colonne verticale lors du parcours de l'orthoimage,
// c'est à dire que ses coordonnees projetées sur le plan sont dans le carré couvert par le pixel de l'orthoimage.
// retourne -1 si aucun point n'a été trouvé dans la colonne verticale
int XPlanProcess::cherchePixelCompatible(std::vector<std::vector<float>> & pointsProjetesSurPlanEnCoordonneesOrthoTranslate, int colonneEnCours, int ligneEnCours, float taillePixelColonne, float taillePixelLigne)
{
	std::vector<float> point;
	for (uint32 i = 0; i < pointsProjetesSurPlanEnCoordonneesOrthoTranslate.size(); i++){
		point = pointsProjetesSurPlanEnCoordonneesOrthoTranslate[i];
		if (point[0] >= (taillePixelColonne * colonneEnCours) && point[0] <= (taillePixelColonne * (colonneEnCours + 1)) && point[1] >= (taillePixelLigne * ligneEnCours) && point[1] <= (taillePixelLigne * (ligneEnCours + 1)))
		{
			return i;
		}
	}
	return -1;
}


// Fonction qui permet de proceder à la correlation. Pris de l'exemple d'openCV sur la corrélation.
int XPlanProcess::detectionCentreCible(std::string imageFileName, std::string referenceFileName)
{
	/// Load image and template
	img = imread(imageFileName, 1);
	templ = imread(referenceFileName, 1);

	if (display == 1)
	{
		/// Create windows
		namedWindow(image_window, CV_WINDOW_AUTOSIZE);
		namedWindow(result_window, CV_WINDOW_AUTOSIZE);

		/// Create Trackbar
		char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
		createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);
	}
	

	MatchingMethod(0, 0);

	std::cout << "Centre de la cible trouve en : "<<matchloctrouve.x << " " << matchloctrouve.y <<" sur l orthoimage." << std::endl;
	if (display == 1)
	{
		waitKey(0);
	}
	return 0;
}

/**
* @function MatchingMethod
* @brief Trackbar callback issue de l'exemple d'openCV
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

	if (display == 1)
	{
		/// Show me what you got
		rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
		rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

		imshow(image_window, img_display);
		imshow(result_window, result);
	}
	

	

	matchloctrouve = cv::Point2d(matchLoc.x + templ.cols / 2, matchLoc.y + templ.rows / 2);
	return;
}
