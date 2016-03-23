#include "XPlanProcess.h"


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

std::vector<float> XPlanProcess::projectionAuPlan(std::vector<float> parametrePlan, std::vector<float> pointM)
{
	float t = -(parametrePlan[0] * pointM[0] + parametrePlan[1] * pointM[1] + parametrePlan[2] * pointM[2] - parametrePlan[3]) / (parametrePlan[0] * parametrePlan[0] + parametrePlan[1] * parametrePlan[1] + parametrePlan[2] * parametrePlan[2]);

	std::vector<float> MPlan;
	MPlan.push_back(pointM[0] + parametrePlan[0] * t);
	MPlan.push_back(pointM[1] + parametrePlan[1] * t);
	MPlan.push_back(pointM[2] + parametrePlan[2] * t);

	return MPlan;

}
