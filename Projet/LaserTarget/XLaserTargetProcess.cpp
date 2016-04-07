#include "XLaserTargetProcess.h"
#include "XArchiXMLException.h"
#include "XArchiXMLBaseTools.h"
#include "XArchiXMLTools.h"

#include "XSystemInfo.h"
#include "XPath.h"
#include "XPlyNuage.h"
#include "XPlanProcess.h"
#include "XRawImage.h"

#include <Eigen/Dense>

//-----------------------------------------------------------------------------
XLaserTargetParams::XLaserTargetParams(void)
{
}
//-----------------------------------------------------------------------------
bool XLaserTargetParams::CheckValid(XError* error)
{
	XPath P;
	P.AddPathSep(input_path);
	P.AddPathSep(output_path);
	if(input_path == output_path)
		XErrorAlert(error,__FUNCTION__,"ATTENTION : Repertoires entree/sortie identiques ");

	return true;
}

//-----------------------------------------------------------------------------
bool XLaserTargetParams::XmlWrite(std::ofstream* out)
{
	*out << "<LaserTarget_params>\n";

	*out << "<input_path>" << input_path << "</input_path>\n";
	*out << "<input_type>" << input_type << "</input_type>\n";
	*out << "<cor_inputfile>" << cor_inputfile << "</cor_inputfile>\n";
	*out << "<output_path>" << output_path << "</output_path>\n";
	*out << "</LaserTarget_params>\n";
	return true;
}
//-------------------------------------------------------------------------
bool XLaserTargetParams_LoadFileXml(XLaserTargetParams* params,std::string filename)
{
	char message[1024];
	//validation du formalisme XML
	TiXmlDocument doc( filename.c_str() );
	if (!doc.LoadFile() ){
		sprintf( message,"Format XML non reconnu. Error='%s'\n", doc.ErrorDesc() );
		throw(XArchiXML::XmlException(message,XArchiXML::XmlException::eBadFormat));
		return false;
	}

	TiXmlElement* root = doc.RootElement();
	XArchiXML::AssertRoot(root,"LaserTarget_params");

	params->input_path = XArchiXML::ReadAssertNodeAsString(root,std::string("input_path"));
	params->input_type = XArchiXML::ReadAssertNodeAsString(root,std::string("input_type"));
	
	params->output_path = XArchiXML::ReadAssertNodeAsString(root,std::string("output_path"));
	params->cor_inputfile = XArchiXML::ReadAssertNodeAsString(root,std::string("cor_inputfile"));
	
	return true;
}


//-----------------------------------------------------------------------------
//***********************    XLaserTargetProcess    *******************************
//-----------------------------------------------------------------------------
XLaserTargetProcess::XLaserTargetProcess(XError* error)
{
	m_error = error;
	m_Laser = new XFileLaser(m_error);
}
//-----------------------------------------------------------------------------
XLaserTargetProcess::~XLaserTargetProcess(void)
{
}
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::LoadXmlFile(std::string filename)
{
	try
	{
		XLaserTargetParams_LoadFileXml(&m_params,filename);
	}
	catch(XArchiXML::XmlException e)
	{
		return XErrorError(m_error,__FUNCTION__,e.Erreur().c_str(),filename.c_str());
	}

	return m_params.CheckValid(m_error);
}
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::InitProcess()
{
	char message[1024];
	XErrorCommentaire(m_error,__FUNCTION__,"Chargement du fichier cor");
	if(!m_Laser->Load(m_params.cor_inputfile))
		return false;
	sprintf(message,"Nombre de points dans le fichier cor: %d",m_Laser->NbPoint());
	XErrorInfo(m_error,__FUNCTION__,message);

	if(Params()->input_type.empty())
		return XErrorError(m_error,__FUNCTION__,"Type en entrée non renseigné ");

	sprintf(message,"Repertoire en entree : %s",m_params.input_path.c_str());
	XErrorCommentaire(m_error,__FUNCTION__,message);

	std::string filtre = "*."+ Params()->input_type;
	systm.GetFileListInFolder(Params()->input_path,m_listFileToProcess,filtre.c_str());
	if(m_listFileToProcess.empty())
		return XErrorError(m_error,__FUNCTION__,"Aucun fichier au format specifie " ,filtre.c_str());
	sprintf(message,"Nombre de fichier a traiter : %d",m_listFileToProcess.size());
	XErrorInfo(m_error,__FUNCTION__,message);

	sprintf(message,"Repertoire de sortie : %s",m_params.output_path.c_str());
	XErrorCommentaire(m_error,__FUNCTION__,message);
	return systm.CreateMultiDirectory(m_params.output_path.c_str());
}
//-----------------------------------------------------------------------------
//STATIC 
std::ofstream* XLaserTargetProcess::out;
uint32 XLaserTargetProcess::count;
XLaserTargetParams XLaserTargetProcess::m_params;
std::map<XLaserPoint*, std::vector<XPlyPoint*>> XLaserTargetProcess::vec3Ply;
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::Process()
{
	InitLaserTarget();
	char message[1024];
	for(uint32 i=0; i< m_listFileToProcess.size(); i++)
	{
		sprintf(message,"\nTraitement %d/%d : ",i+1,m_listFileToProcess.size());
		XErrorInfo(m_error,__FUNCTION__,message,P.Name(m_listFileToProcess[i].c_str()).c_str());
		ProcessFile(m_listFileToProcess[i]);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::ProcessPlyLaser3ply(XPlyPoint* pt)
{
	XPt3D p1 =pt->Position();
	for( std::map<XLaserPoint*, std::vector<XPlyPoint*>>::iterator iter = vec3Ply.begin(); iter != vec3Ply.end(); iter++ ) 
	{
		XPt3D p2 =*(*iter).first;
		XPlyPoint* newPoint = pt->Clone();
		(*iter).second.push_back(newPoint);
	}
	count++;
	return true;
}
//-----------------------------------------------------------------------------
void XLaserTargetProcess::InitLaserTarget()
{
	std::vector<XPlyPoint*> vecVide;
	for(uint32 i=0; i < m_Laser->NbPoint(); i++)
		vec3Ply[m_Laser->Point(i)] = vecVide;
		
}
//-----------------------------------------------------------------------------
void XLaserTargetProcess::ClearLaserTarget()
{
	std::vector<XPlyPoint*> vecVide;
	for( std::map<XLaserPoint*, std::vector<XPlyPoint*>>::iterator iter = vec3Ply.begin(); iter != vec3Ply.end(); iter++ ) 
	{
		for(uint32 i=0; i < (*iter).second.size(); i++)
			delete (*iter).second[i];
		(*iter).second.clear();
		(*iter).second.swap(vecVide);
	}
}
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::ProcessFile(std::string filename)
{
	XPlyNuage plyIn(filename,m_error);
	if(!plyIn.ReadEntete())
		return false;

	ClearLaserTarget();

	F_PlyProcessPoint fp = &this->ProcessPlyLaser3ply;
	if(!plyIn.ReadDatas(fp))
		return XErrorError(m_error,__FUNCTION__,"Erreur de traitement ",filename.c_str());	

	char message[1024];
	std::string plyinput = P.NameNoExt(filename.c_str());

	std::map<XLaserPoint*, std::vector<XPlyPoint*>>::iterator iter = vec3Ply.begin();

	std::string cas("sphère");

	if (cas.compare("cible") == 0)
	{
		std::vector<std::vector<float>> points;

		for (uint32 i = 0; i < (*iter).second.size(); i++)
		{
			XPlyPoint* pt = (*iter).second[i]; // Point i 
			XPt3D p1 = pt->Position();
			std::vector<float> coordonnees;
			// Acceder au x, y, z
			float x = p1.X;
			float y = p1.Y;
			float z = p1.Z;
			coordonnees.push_back(x);
			coordonnees.push_back(y);
			coordonnees.push_back(z);
			points.push_back(coordonnees);


		}

		XPlanProcess detection;
		std::vector<float> parametrePlan = detection.determinationEquationPlan(points);

		std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesTerrain;
		std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho;
		std::vector<float> carteProfondeur;
		std::vector<float> normal;
		normal.push_back(parametrePlan[0]);
		normal.push_back(parametrePlan[1]);
		normal.push_back(parametrePlan[2]);

		for (uint32 i = 0; i < points.size(); i++)
		{
			pointsProjetesSurPlanEnCoordonneesTerrain.push_back(detection.projectionAuPlan(parametrePlan, points[i]));
			carteProfondeur.push_back(detection.profondeurAuPlan(normal, pointsProjetesSurPlanEnCoordonneesTerrain[i], points[i]));
		}
		detection.constructionBase();
		detection.matricePassageReferentielLaserVersReferentielOrthoimage();
		detection.matricePassageReferentielOrthoimageVersReferentielLaser();

		pointsProjetesSurPlanEnCoordonneesOrtho = detection.changementEnBaseOrtho(pointsProjetesSurPlanEnCoordonneesTerrain);

		std::vector<float> bornes = detection.bornesDeLOrtho(pointsProjetesSurPlanEnCoordonneesOrtho);

		std::vector<std::vector<float>>pointsProjetesSurPlanEnCoordonneesOrthoTranslate;
		pointsProjetesSurPlanEnCoordonneesOrthoTranslate = detection.changementOrigine(pointsProjetesSurPlanEnCoordonneesOrtho, bornes);


		// Generation de l'orthoimage
		// Configuration de l'orthoimage
		int colonne = 25;
		int ligne = 25;
		// Equivalence de la taille d'un pixel en coordonnees terrain
		float taillePixelLigne = (bornes[3] - bornes[1]) / ligne;
		float taillePixelColonne = (bornes[2] - bornes[0]) / colonne;

		// Recuperation des valeurs extremales de l'intensite du nuage de point
		// Initialisation de ces valeurs
		XPlyPoint* pt = (*iter).second[0];
		float p0 = pt->Intensity();
		float minIntensite = p0;
		float maxIntensite = p0;

		for (uint32 i = 0; i < (*iter).second.size(); i++)
		{
			XPlyPoint* pt = (*iter).second[i]; // Point i 
			float p1 = pt->Intensity();
			if (p1 < minIntensite)
			{
				minIntensite = p1;
				continue;
			}
			if (p1 > maxIntensite)
			{
				maxIntensite = p1;
			}
		}

		// Calcul du ratio pour normaliser la valeur d'intensite entre 0 et 255
		float ratio = 255 / (maxIntensite + abs(minIntensite));

		// Coloriage de l'orthoimage
		XRawImage MonoImage(colonne, ligne, 8, 1, NULL);
		byte* pix = MonoImage.Pixels();
		memset(pix, 0, colonne*ligne);

		// Image qui va stocker l'identifiant du point du nuage selectionné pour colorier l'image (utile pour les transformations inverses)
		XRawImage ImageIndiceCorrespondante(colonne, ligne, 8, 1, NULL);
		byte* pix_ImageIndiceCorrespondante = ImageIndiceCorrespondante.Pixels();
		memset(pix_ImageIndiceCorrespondante, -1, colonne*ligne);

		// Coloriage
		int i = ligne * colonne;
		for (int ligneEnCours = 0; ligneEnCours < ligne; ligneEnCours++)
		{
			for (int colonneEnCours = 0; colonneEnCours < colonne; colonneEnCours++)
			{
				// Detection du point qui va servir pour colorier ce pixel
				int indicePointAAjouter = detection.cherchePixelCompatible(pointsProjetesSurPlanEnCoordonneesOrthoTranslate, colonneEnCours, ligneEnCours, taillePixelColonne, taillePixelLigne);

				// Si un point correspond on normalise son intensité et on rajoute son intensite dans l'ortho et son indice dans l'autre image
				if (indicePointAAjouter != -1)
				{
					XPlyPoint* pt2 = (*iter).second[indicePointAAjouter]; // Point i
					float p1 = pt2->Intensity();
					*pix = (unsigned char)floor((p1 + abs(minIntensite))*ratio);
					*pix_ImageIndiceCorrespondante = indicePointAAjouter;
				}

				std::cout << i << std::endl;
				i--;
				pix++;
			}
		}

		// Ecriture de l'image
		XPath P;

		std::string file = m_params.output_path + plyinput.c_str() + "_ortho_" + std::to_string(colonne) + "x" + std::to_string(ligne) + ".tif";
		MonoImage.WriteFile(file.c_str());

		std::string indiceOrtho = m_params.output_path + plyinput.c_str() + "_IndiceOrtho_" + std::to_string(colonne) + "x" + std::to_string(ligne) + ".tif";
		ImageIndiceCorrespondante.WriteFile(indiceOrtho.c_str());
	}


	// Récuperation des points de couleur dans le cas d'une sphère
	if (cas.compare("sphère") == 0)
	{
		std::cout << "Cas : sphere" << std::endl;
		std::string couleur("vert");
		std::vector<std::vector<float>> points_color;

		for (uint32 i = 0; i < (*iter).second.size(); i++)
		{
			XPlyPoint* pt = (*iter).second[i]; // Point i 
			float rouge = (float)pt->R();
			float vert = (float)pt->G();
			float bleu = (float)pt->B();

			XPt3D p = pt->Position();

			if (couleur.compare("vert") == 0){
				if ((vert / rouge) > 1.2 && (vert / bleu) > 1.2)
				{
					// Acceder au x, y, z
					std::vector<float> coordonnees_color;
					float x = p.X;
					float y = p.Y;
					float z = p.Z;
					coordonnees_color.push_back(x);
					coordonnees_color.push_back(y);
					coordonnees_color.push_back(z);
					points_color.push_back(coordonnees_color);
				}
			}
			if (couleur.compare("rouge") == 0){
				if ((rouge / vert) > 1.2 && (rouge / bleu) > 1.2)
				{
					// Acceder au x, y, z
					std::vector<float> coordonnees_color;
					float x = p.X;
					float y = p.Y;
					float z = p.Z;
					coordonnees_color.push_back(x);
					coordonnees_color.push_back(y);
					coordonnees_color.push_back(z);
					points_color.push_back(coordonnees_color);
				}
			}
			if (couleur.compare("bleu") == 0){
				if ((bleu / rouge) > 1.2 && (bleu / vert) > 1.2)
				{
					// Acceder au x, y, z
					std::vector<float> coordonnees_color;
					float x = p.X;
					float y = p.Y;
					float z = p.Z;
					coordonnees_color.push_back(x);
					coordonnees_color.push_back(y);
					coordonnees_color.push_back(z);
					points_color.push_back(coordonnees_color);
				}
			}
		}
		// Fichir ply A FAIRE

		// Test sur le nombre de pixels colorés
		if (points_color.size() > 120)
		{
			// Moyenne des points de couleurs
			int n = 0;
			float x = 0;
			float y = 0;
			float z = 0;
			float x_, y_, z_;
			for (uint32 i = 0; i < points_color.size(); i++)
			{
				n += 1;
				x += points_color[i][0];
				y += points_color[i][1];
				z += points_color[i][2];
			}
			x_ = x / n;
			y_ = y / n;
			z_ = z / n;


			// Cordonnées de la station
			XPt3D* station = (*iter).first;
			float x_sta = (float)station->X;
			float y_sta = (float)station->Y;
			float z_sta = (float)station->Z;

			// Vecteur directeur entre la station laser et le point 
			std::vector<float> vecteur_directeur;
			float x_vec = x_ - x_sta;
			float y_vec = y_ - y_sta;
			float z_vec = z_ - z_sta;
			float norme_st = sqrt(x_vec*x_vec + y_vec*y_vec + z_vec*z_vec);
			vecteur_directeur.push_back(x_vec / norme_st);
			vecteur_directeur.push_back(y_vec / norme_st);
			vecteur_directeur.push_back(z_vec / norme_st);

			float rayon = 2.5;
			float x_appr = x_ + rayon*vecteur_directeur[0];
			float y_appr = y_ + rayon*vecteur_directeur[1];
			float z_appr = z_ + rayon*vecteur_directeur[2];
		}

	}
	
	/*
	for( std::map<XLaserPoint*, std::vector<XPlyPoint*>>::iterator iter = vec3Ply.begin(); iter != vec3Ply.end(); iter++ ) 
	{
		
		if((*iter).second.size() > m_params.min_nb_point_out)
		{
			std::string outfilename = m_params.output_path + XPath::PathSep + (*iter).first->Id + "_" + plyinput + ".ply";
			sprintf(message,"%s --> %d pts", P.NameNoExt(outfilename.c_str()).c_str(),(*iter).second.size());
			XErrorInfo(m_error,__FUNCTION__,outfilename.c_str());
			XErrorInfo(m_error,__FUNCTION__,message);
			XPlyNuage plyOut(outfilename,m_error);
			plyOut.Station(plyIn.Station());
			plyOut.InitOutput(plyIn.ptCourant,(*iter).second.size());
			for(uint32 i=0; i< (*iter).second.size(); i++)
				(*iter).second[i]->WriteBinary(plyOut.out);
			plyOut.out->close();
		}
		
		
	}
	*/

return true;	
}

