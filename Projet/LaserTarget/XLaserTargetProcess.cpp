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
#include <iostream>
#include <string>
#include <fstream>


//-----------------------------------------------------------------------------
//***********************    XLaserTargetParams    ****************************
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
	params->txt_inputfile = XArchiXML::ReadAssertNodeAsString(root, std::string("txt_inputfile"));
	
	return true;
}

//-----------------------------------------------------------------------------
//***********************    XLaserTargetModel    ****************************
//-----------------------------------------------------------------------------
XLaserTargetModel::XLaserTargetModel(void)
{
}
//-------------------------------------------------------------------------
bool XLaserTargetModel_LoadFileXml(XLaserTargetModel* datas, std::string filename)
{
	char message[1024];
	//validation du formalisme XML
	TiXmlDocument doc(filename.c_str());
	if (!doc.LoadFile()){
		sprintf(message, "Format XML non reconnu. Error='%s'\n", doc.ErrorDesc());
		throw(XArchiXML::XmlException(message, XArchiXML::XmlException::eBadFormat));
		return false;
	}

	//TiXmlElement* root = doc.RootElement();
	//XArchiXML::AssertRoot(root, "LaserTarget_model");

	TiXmlHandle hdl(&doc);
	TiXmlElement *path = hdl.FirstChildElement("LaserTarget_model").FirstChildElement("DataBase").FirstChildElement("pathDataBase").Element();
	datas->data_path = path->GetText();

	TiXmlElement *elem = hdl.FirstChildElement("LaserTarget_model").FirstChildElement("DataBase").FirstChildElement("model").Element();

	model mod;

	if (!elem){
		std::cout << "Erreur: Fichier model erroné" << std::endl;
		return 0;
	}
	while (elem){
		/*std::string type = elem->Attribute("class");
		datas->type.push_back(elem->Attribute("class"));
		datas->id.push_back(elem->Attribute("id"));
		datas->nom.push_back(elem->Attribute("nom"));
		datas->image2D.push_back(elem->Attribute("image2D"));
		datas->resolution.push_back(elem->Attribute("resolution"));*/
		mod.type = elem->Attribute("class");
		elem->QueryIntAttribute("id", &mod.id);
		mod.nom = elem->Attribute("nom");
		mod.image2D = elem->Attribute("image2D");
		elem->QueryFloatAttribute("resolution", &mod.resolution);

		//TiXmlElement* root = doc.RootElement();
		//XArchiXML::AssertRoot(root, "model");

		if (mod.type.compare("cible") == 0)
		{
			/*datas->rayon.push_back(elem->Attribute(""));
			datas->couleur.push_back("");
			datas->nuage.push_back("");*/
			mod.rayon = NULL;
			mod.couleur = "";
			mod.nuage = "";
			
		}
		if (mod.type.compare("sphere") == 0)
		{
			/*datas->rayon.push_back(elem->Attribute("rayon"));
			datas->image2D.push_back(elem->Attribute("couleur"));
			datas->image2D.push_back(elem->Attribute("nuage"));*/
			elem->QueryFloatAttribute("rayon", &mod.rayon);
			mod.couleur = elem->Attribute("couleur");
			mod.nuage = elem->Attribute("nuage");;
		}

		datas->model_list.push_back(mod);
		//sprintf(message, "%s", mod.nom);
		elem = elem->NextSiblingElement(); // iteration 
	}
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
bool XLaserTargetProcess::LoadXmlData(std::string filename)
{
	try
	{
		XLaserTargetModel_LoadFileXml(&m_datas, filename);
	}
	catch (XArchiXML::XmlException e)
	{
		return XErrorError(m_error, __FUNCTION__, e.Erreur().c_str(), filename.c_str());
	}

	return true;
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
XLaserTargetModel XLaserTargetProcess::m_datas;
std::map<XLaserPoint*, std::vector<XPlyPoint*>> XLaserTargetProcess::vec3Ply;
std::vector<int> XLaserTargetProcess::liste_txt_id;
std::vector<std::string> XLaserTargetProcess::liste_txt_nom;
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
	std::string cas("sphere");
	std::string couleur("vert");
	
	XPt3D p1 =pt->Position();
	for( std::map<XLaserPoint*, std::vector<XPlyPoint*>>::iterator iter = vec3Ply.begin(); iter != vec3Ply.end(); iter++ ) 
	{
		XPt3D p2 =*(*iter).first;
		XPlyPoint* newPoint = pt->Clone();
		if (cas.compare("sphere") == 0){
			float rouge = (float)pt->R();
			float vert = (float)pt->G();
			float bleu = (float)pt->B();

			XPt3D p = pt->Position();

			if (couleur.compare("vert") == 0){
				if ((vert / rouge) > 1.3 && (vert / bleu) > 1.3)
				{
					(*iter).second.push_back(newPoint);
				}
			}
			else if (couleur.compare("rouge") == 0){
				if ((rouge / vert) > 1.1 && (rouge / bleu) > 1.1)
				{
					(*iter).second.push_back(newPoint);
				}
			}
			else if (couleur.compare("bleu") == 0){
				if ((bleu / rouge) > 1 && (bleu / vert) > 1)
				{
					(*iter).second.push_back(newPoint);
				}
			}
		}
		else {
			(*iter).second.push_back(newPoint);
		}
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

//-----------------------------------------------------------------------------
bool XLaserTargetProcess::LoadCibleUSphere(std::string filename)
{
	std::ifstream fichier(filename, std::ios::in);  // on ouvre le fichier en lecture

	if (fichier)  // si l'ouverture a réussi
	{
		// instructions
		std::string ligne;
		std::vector<std::string > temp;
		while (getline(fichier, ligne))  // tant que l'on peut mettre la ligne dans "contenu"
		{
			std::istringstream iss(ligne);
			std::string str;
			while (std::getline(iss, str, ' '))
			{
				temp.push_back(str);
			}
			if (temp.size() != 2){
				sprintf("Erreur des données dans : %s : ", filename.c_str());
			}
			liste_txt_nom.push_back(temp[0]);
			liste_txt_id.push_back(atoi(temp[1].c_str()));
			temp.clear();
		}
		fichier.close();  // on ferme le fichier
	}
	else  // sinon
		sprintf("Non lecture de : %s : ", filename.c_str());

	return true;
}
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

	// String to split.
	std::string to_split = plyinput;
	// The result will be stored in [result].
	std::vector<std::string> fichier;
	std::istringstream iss(to_split);
	std::string str;
	while (std::getline(iss, str, '_'))
	{
		fichier.push_back(str);
	}
	// Nom de la cible/sphère : fichier[0]
	std::string cas;
	int cas_id;
	//Trouver dans quel cas on est
	for (int i = 0; i < liste_txt_nom.size(); i++){
		if (liste_txt_nom[i].compare(fichier[0]) == 0){
			cas_id = liste_txt_id[i];
		}
	}
	for (std::_List_iterator<std::_List_val<std::_List_simple_types<model>>> mod = Data()->model_list.begin(); mod != Data()->model_list.end(); mod++){
		//std::list<model, std::allocator<model>> mod = Data()->model_list.begin();
		//std::_List_iterator<std::_List_val<std::_List_simple_types<model>>>
		int id = mod->id;
		if (id == cas_id){
			cas = mod->type;
		}
	}

	//std::string cas("sphere");

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
		/*int colonne = 100;
		int ligne = 100;
		// Equivalence de la taille d'un pixel en coordonnees terrain
		float taillePixelLigne = (bornes[3] - bornes[1]) / ligne;
		float taillePixelColonne = (bornes[2] - bornes[0]) / colonne;
		*/
		float taillePixelLigne = 0.1;
		float taillePixelColonne = 0.1;

		int ligne = (bornes[3] - bornes[1]) / taillePixelLigne;
		int colonne = (bornes[2] - bornes[0]) / taillePixelColonne;










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
		int indicePointAAjouter;
		for (int ligneEnCours = 0; ligneEnCours < ligne; ligneEnCours++)
		{
			for (int colonneEnCours = 0; colonneEnCours < colonne; colonneEnCours++)
			{
				// Detection du point qui va servir pour colorier ce pixel
				indicePointAAjouter = detection.cherchePixelCompatible(pointsProjetesSurPlanEnCoordonneesOrthoTranslate, colonneEnCours, ligneEnCours, taillePixelColonne, taillePixelLigne);

				// Si un point correspond on normalise son intensité et on rajoute son intensite dans l'ortho et son indice dans l'autre image
				if (indicePointAAjouter != -1)
				{
					XPlyPoint* pt2 = (*iter).second[indicePointAAjouter]; // Point i
					float p1 = pt2->Intensity();
					*pix = (unsigned char)floor((p1 + abs(minIntensite))*ratio);
					*pix_ImageIndiceCorrespondante = (unsigned char)indicePointAAjouter;
				}

				std::cout << i << " " << indicePointAAjouter  << std::endl;
				i--;
				pix++;
				pix_ImageIndiceCorrespondante++;
			}
		}

		// Ecriture de l'image
		XPath P;
		std::string taillePixelColonneString = std::to_string(taillePixelColonne);
		std::string taillePixelLigneString = std::to_string(taillePixelLigne);
		std::replace(taillePixelColonneString.begin(), taillePixelColonneString.end(), '.', '_');
		std::replace(taillePixelLigneString.begin(), taillePixelLigneString.end(), '.', '_');
		std::string file = m_params.output_path + plyinput.c_str() + "_ortho_" + taillePixelColonneString + "x" + taillePixelLigneString + ".tif";
		MonoImage.WriteFile(file.c_str());
		
		//std::string file = m_params.output_path + plyinput.c_str() + "_ortho_" + std::to_string(colonne) + "x" + std::to_string(ligne) + ".tif";
		
		std::string indiceOrtho = m_params.output_path + plyinput.c_str() + "_IndiceOrtho_" + taillePixelColonneString + "x" + taillePixelLigneString + ".tif";
		ImageIndiceCorrespondante.WriteFile(indiceOrtho.c_str());

		std::string reference = m_params.output_path + "referenceCiblette_ortho_0_100000x0_100000.tif";
		detection.detectionCentreCible(file, reference);


		// Récupération de la valeur d'un pixel
		int identifiantPoint = detection.cherchePixelCompatible(pointsProjetesSurPlanEnCoordonneesOrthoTranslate, detection.matchloctrouve.x, detection.matchloctrouve.y, taillePixelColonne, taillePixelLigne);
		if (identifiantPoint != -1)
		{
			XPlyPoint* pt2 = (*iter).second[identifiantPoint]; // Point i
			float p1 = pt2->Intensity();
			std::cout << p1 << " " << (float)pt2->R() << " " << (float)pt2->B() << " " << (float)pt2->G() << " ok" << std::endl;
			
		}
		//int identifiantPoint = detection.chercheValeurPixel(indiceOrtho.c_str(), detection.matchloctrouve.x, detection.matchloctrouve.y);
	}


	// Récuperation des points de couleur dans le cas d'une sphère
	if (cas.compare("sphere") == 0)
	{
		
		std::string outfilename = m_params.output_path + XPath::PathSep + "boule_" + plyinput + ".ply";
		sprintf(message, "%s --> %d pts", P.NameNoExt(outfilename.c_str()).c_str(), (*iter).second.size());
		XErrorInfo(m_error, __FUNCTION__, outfilename.c_str());
		XErrorInfo(m_error, __FUNCTION__, message);
		XPlyNuage plyOut(outfilename, m_error);
		plyOut.Station(plyIn.Station());
		plyOut.InitOutput(plyIn.ptCourant, (*iter).second.size());
		for (uint32 i = 0; i< (*iter).second.size(); i++)
			(*iter).second[i]->WriteBinary(plyOut.out);
		plyOut.out->close();

		std::cout << "Cas : sphere" << std::endl;
		/*std::string couleur("vert");
		std::vector<std::vector<float>> points_color;*/

		/*for (uint32 i = 0; i < (*iter).second.size(); i++)
		{
			Modification de la création de iter
			Supprimer le ligne suivante 
			Appel au cas couleur que dans le création de l'iter
			Supprimer cas couleur dans ce process
			Modifier la moyen à faire sur tout l'iter
			Supression des variables en trop
			
			XPlyPoint* pt = (*iter).second[i]; // Point i 
			float rouge = (float)pt->R();
			float vert = (float)pt->G();
			float bleu = (float)pt->B();

			XPt3D p = pt->Position();

			if (couleur.compare("vert") == 0){
				if ((vert / rouge) > 1 && (vert / bleu) > 1)
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
			else if (couleur.compare("rouge") == 0){
				if ((rouge / vert) > 1 && (rouge / bleu) > 1)
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
			else if (couleur.compare("bleu") == 0){
				if ((bleu / rouge) > 1 && (bleu / vert) > 1)
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
			else{
			}
		}*/
		// Fichier ply fait

		// Test sur le nombre de pixels colorés
		if ((*iter).second.size() > 120) //if (points_color.size() > 120)
		{
			// Moyenne des points de couleurs
			int n = 0;
			float x = 0, y = 0, z = 0;
			float x_, y_, z_;
			for (uint32 i = 0; i < (*iter).second.size(); i++)
			{
				XPlyPoint* pt = (*iter).second[i]; // Point i 
				XPt3D p = pt->Position();

				n += 1;
				x += p.X; //points_color[i][0];
				y += p.Y; // points_color[i][1];
				z += p.Z; // points_color[i][2];
			}
			x_ = x / n;
			y_ = y / n;
			z_ = z / n;

			// Cordonnées de la station
			float x_sta = (float)plyIn.m_Station.X;
			float y_sta = (float)plyIn.m_Station.Y;
			float z_sta = (float)plyIn.m_Station.Z;

			// Vecteur directeur entre la station laser et le point 
			std::vector<float> vecteur_directeur;
			float x_vec = x_ - x_sta;
			float y_vec = y_ - y_sta;
			float z_vec = z_ - z_sta;
			float norme_st = sqrt(x_vec*x_vec + y_vec*y_vec + z_vec*z_vec);
			vecteur_directeur.push_back(x_vec / norme_st);
			vecteur_directeur.push_back(y_vec / norme_st);
			vecteur_directeur.push_back(z_vec / norme_st);

			float rayon = 0.025;
			float x_appr = x_ + rayon*vecteur_directeur[0];
			float y_appr = y_ + rayon*vecteur_directeur[1];
			float z_appr = z_ + rayon*vecteur_directeur[2];

			int taille = (*iter).second.size();
			//Boucle
			float xr = x_appr;
			float yr = y_appr;
			float zr = z_appr;
			// Moindres carrés
			// Y = AX
			// A
			Eigen::Matrix<double, Eigen::Dynamic, 3> A;
			A.resize((*iter).second.size(), 3); 
			//Eigen::MatrixXf A((*iter).second.size(), 3);
			//Eigen::Matrix<float, 1, 3> B;
			// Y
			Eigen::Matrix<double, Eigen::Dynamic, 1> Y;
			Y.resize((*iter).second.size(), 1);
			//Eigen::MatrixXf Y((*iter).second.size(),3);
			// P
			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> P;
			P.resize((*iter).second.size(), (*iter).second.size());
			//Eigen::MatrixXf P((*iter).second.size(), (*iter).second.size());
			
			for (uint32 k = 0; k < 2; k++)
			{
				
				// Construction
				for (uint32 j = 0; j < (*iter).second.size(); j++)
				{
					XPlyPoint* pt = (*iter).second[j]; // Point j 
					XPt3D p = pt->Position();

					float d0 = sqrt(pow(p.X - xr, 2) + pow(p.Y - yr, 2) + pow(p.Z - zr, 2));
					A.row(j) << (xr - p.X) / d0, (yr - p.Y) / d0, (zr - p.Z) / d0;
					Y.row(j) << rayon - d0;
					//P.row(j).col(j) << 1;
				}
				
				
				//P = (1 / 0.01)*P;
				// X
				Eigen::Vector3d X;
				//Eigen::Matrix3f B;
				//Eigen::Matrix3f B_inv;
				//Eigen::Matrix<float, 3, 100> A_tr;
				//Eigen::Matrix<float, 3, 100> C;
				//A_tr = A.transpose();
				//B = A_tr*A;
				//B_inv = B.inverse();
				//C = B_inv*A_tr;
				//X = C*Y;
				X = ((((A.transpose())*A).inverse())*(A.transpose()))*Y;
				//X = ((((((A.transpose())*P)*A).inverse())*(A.transpose()))*P)*Y;
				xr += X(0, 0);
				yr += X(1, 0);
				zr += X(2, 0);

				//Vecteur résidu
				Eigen::Matrix<double, Eigen::Dynamic, 1> V;
				V.resize((*iter).second.size(), 1);
				//Eigen::Matrix<float, 100, 1> V;
				V = Y - A*X;
				//Facteur de variance
				float sigma2_m = ((V.transpose())*V);
				float sigma2 = sigma2_m*(1 / (100 - 3));
				//float sigma2_m = (((V.transpose())*P)*V);
				//float sigma2 = sigma2_m*(1 / (100 - 3));
				
				//NE CONVERGE PAS VERS LES BONNES VALEURS
			}
		}

	}

	// Coordonnées du point par rapport à la station
	
	
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
	//Ecriture des fichiers de sorties


	std::string fichierSortie = m_params.output_path + "pointAppui.XYZ";
	std::ofstream monflux(fichierSortie.c_str()); //déclaration du flux et ouverture du fichier 

	if (monflux)
	{
		int X = 20;
		int Y = 20;
		int Z = 20;
		monflux << "Coordonnées exactes du point d'appui dans le système laser : " << std::endl;
		monflux << X << '  ' << Y << '  ' << Z;

		monflux.close();
	}
	else
	{
		std::cerr << "Impossible d'ouvrir le fichier !" << std::endl;
	}
return true;	
}

