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

	TiXmlHandle hdl(&doc);
	TiXmlElement *path = hdl.FirstChildElement("LaserTarget_model").FirstChildElement("DataBase").FirstChildElement("pathDataBase").Element();
	datas->data_path = path->GetText();

	TiXmlElement *elem = hdl.FirstChildElement("LaserTarget_model").FirstChildElement("DataBase").FirstChildElement("model").Element();

	model mod;

	if (!elem){
		std::cout << "Erreur: Fichier model erron�" << std::endl;
		return 0;
	}
	while (elem){
		mod.type = elem->Attribute("class");
		elem->QueryIntAttribute("id", &mod.id);
		mod.nom = elem->Attribute("nom");
		mod.image2D = elem->Attribute("image2D");
		elem->QueryFloatAttribute("resolution", &mod.resolution);

		if (mod.type.compare("cible") == 0)
		{
			mod.rayon = NULL;
			mod.couleur = "";
			mod.nuage = "";
			
		}
		if (mod.type.compare("sphere") == 0)
		{
			elem->QueryFloatAttribute("rayon", &mod.rayon);
			mod.couleur = elem->Attribute("couleur");
			mod.nuage = elem->Attribute("nuage");;
		}

		datas->model_list.push_back(mod);
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
		return XErrorError(m_error,__FUNCTION__,"Type en entr�e non renseign� ");

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
std::string XLaserTargetProcess::cas;
std::string XLaserTargetProcess::couleur;
std::string XLaserTargetProcess::image2D;
float XLaserTargetProcess::rayon; 
float XLaserTargetProcess::resolution;
//-----------------------------------------------------------------------------
bool XLaserTargetProcess::Process()
{
	InitLaserTarget();
	char message[1024];
	LoadCibleUSphere(Params()->txt_inputfile);
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
	//couleur = "vert";
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

	if (fichier)  // si l'ouverture a r�ussi
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
				sprintf("Erreur des donn�es dans : %s : ", filename.c_str());
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


// Fonction qui teste si un fichier existe d�j� ou non
inline bool  XLaserTargetProcess::exists_test(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}



bool XLaserTargetProcess::ProcessFile(std::string filename)
{
	XPlyNuage plyIn(filename,m_error);
	if(!plyIn.ReadEntete())
		return false;

	ClearLaserTarget();

	
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
	// Nom de la cible/sph�re : fichier[0]
	//std::string cas;
	int cas_id;
	//Trouver dans quel cas on est
	for (int i = 0; i < liste_txt_nom.size(); i++){
		if (liste_txt_nom[i].compare(fichier[0]) == 0){
			cas_id = liste_txt_id[i];
		}
	}
	for (std::_List_iterator<std::_List_val<std::_List_simple_types<model>>> mod = Data()->model_list.begin(); mod != Data()->model_list.end(); mod++){
		int id = mod->id;
		if (id == cas_id){
			cas = mod->type;
			couleur = mod->couleur;
			image2D = mod->image2D;
			rayon = mod->rayon;
			resolution = mod->resolution;
			break;
		}
	}

	F_PlyProcessPoint fp = &this->ProcessPlyLaser3ply;
	if (!plyIn.ReadDatas(fp))
		return XErrorError(m_error, __FUNCTION__, "Erreur de traitement ", filename.c_str());

	float xr = 0;
	float yr = 0;
	float zr = 0;
	if (cas.compare("cible") == 0)
	{
		std::cout << "Cas : cible" << std::endl;

		std::vector<std::vector<float>> points;

		// On copie les points pour g�n�rer un vecteur contenant que la g�om�trie des points
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

		// D�termination de l'�quation du plan 
		std::vector<float> parametrePlan = detection.determinationEquationPlan(points);

		std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesTerrain;
		std::vector<std::vector<float>> pointsProjetesSurPlanEnCoordonneesOrtho;
		//std::vector<float> carteProfondeur;

		std::vector<float> normal;
		normal.push_back(parametrePlan[0]);
		normal.push_back(parametrePlan[1]);
		normal.push_back(parametrePlan[2]);

		// Projection orthogonale de chaque point sur le plan
		for (uint32 i = 0; i < points.size(); i++)
		{
			pointsProjetesSurPlanEnCoordonneesTerrain.push_back(detection.projectionAuPlan(parametrePlan, points[i]));
			//carteProfondeur.push_back(detection.profondeurAuPlan(normal, pointsProjetesSurPlanEnCoordonneesTerrain[i], points[i]));
		}

		// Creation d'une base orthonorm�e pour traiter le nuage dans le plan local
		detection.constructionBase();

		// G�n�ration des matrices de passage
		detection.matricePassageReferentielLaserVersReferentielOrthoimage();
		detection.matricePassageReferentielOrthoimageVersReferentielLaser();

		// Changement de rep�re du nuage 
		pointsProjetesSurPlanEnCoordonneesOrtho = detection.changementEnBaseOrtho(pointsProjetesSurPlanEnCoordonneesTerrain);

		// Calcul de l'emprise du nuage projet� sur le plan 
		std::vector<float> bornes = detection.bornesDeLOrtho(pointsProjetesSurPlanEnCoordonneesOrtho);

		// Translation de l'origine pour g�n�rer une orthoimage couvrant l'emprise du nuage
		std::vector<std::vector<float>>pointsProjetesSurPlanEnCoordonneesOrthoTranslate;
		pointsProjetesSurPlanEnCoordonneesOrthoTranslate = detection.changementOrigine(pointsProjetesSurPlanEnCoordonneesOrtho, bornes);


		// Generation de l'orthoimage
		// Configuration de l'orthoimage
		
		// Dimension terrain d'un pixel <=> R�solution
		float taillePixelLigne = resolution;
		float taillePixelColonne = resolution;

		std::string taillePixelColonneString = std::to_string(taillePixelColonne);
		std::string taillePixelLigneString = std::to_string(taillePixelLigne);
		std::replace(taillePixelColonneString.begin(), taillePixelColonneString.end(), '.', '_');
		std::replace(taillePixelLigneString.begin(), taillePixelLigneString.end(), '.', '_');
		std::string file = m_params.output_path + plyinput.c_str() + "_ortho_" + taillePixelColonneString + "x" + taillePixelLigneString + ".tif";
		
		// On cr�e l'orthoimage que si elle n'existe pas d�j� (�vite des temps de calculs)		
		if (!exists_test(file))
		{
			try {
				// D�termination du nombre de ligne et de colonne n�cessaire pour �crire l'orthoimage
				int ligne = (bornes[3] - bornes[1]) / taillePixelLigne;
				int colonne = (bornes[2] - bornes[0]) / taillePixelColonne;

				// Recuperation des valeurs extremales de l'intensite des points pour la normaliser sur l'orthoimage
				// Initialisation de ces valeurs
				XPlyPoint* pt = (*iter).second[0];
				float p0 = pt->Intensity();
				float minIntensite = p0;
				float maxIntensite = p0;

				for (uint32 i = 0; i < (*iter).second.size(); i++)
				{
					XPlyPoint* pt = (*iter).second[i];
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
				float ratio = 255 / (maxIntensite - minIntensite);

				// Coloriage de l'orthoimage
				XRawImage MonoImage(colonne, ligne, 8, 1, NULL);
				byte* pix = MonoImage.Pixels();
				memset(pix, 0, colonne*ligne);

				int i = ligne * colonne;
				int indicePointAAjouter;
				for (int ligneEnCours = 0; ligneEnCours < ligne; ligneEnCours++)
				{
					for (int colonneEnCours = 0; colonneEnCours < colonne; colonneEnCours++)
					{
						// Detection du point qui va servir pour colorier ce pixel
						indicePointAAjouter = detection.cherchePixelCompatible(pointsProjetesSurPlanEnCoordonneesOrthoTranslate, colonneEnCours, ligneEnCours, taillePixelColonne, taillePixelLigne);

						// Si un point correspond on normalise son intensit� et on rajoute son intensite dans l'ortho
						if (indicePointAAjouter != -1)
						{
							XPlyPoint* pt2 = (*iter).second[indicePointAAjouter]; 
							float p1 = pt2->Intensity();
							*pix = (unsigned char)floor((p1 + abs(minIntensite))*ratio);
							//// Correction si intensit� minimale pour ne pas apparaitre comme un pixel noir
							//if (floor((p1 + abs(minIntensite))*ratio) == 0)
							//{
							//	*pix = (unsigned char)100;
							//}
						}

						//std::cout << i << " " << indicePointAAjouter << std::endl;
						i--;
						pix++;
					}
				}

				// Ecriture de l'image
				XPath P;
				MonoImage.WriteFile(file.c_str());
			}
			catch (...) {
				XErrorError(m_error, __FUNCTION__, "La generation de l orthoimage a echoue");
			}
		}


		// Correlation avec la r�f�rence
		std::string reference = m_params.output_path + image2D; // Image � retrouver
		try {
			detection.detectionCentreCible(file, reference);

			// R�cup�ration de la valeur d'un pixel
			int identifiantPoint = detection.cherchePixelCompatible(pointsProjetesSurPlanEnCoordonneesOrthoTranslate, detection.matchloctrouve.x, detection.matchloctrouve.y, taillePixelColonne, taillePixelLigne);
			if (identifiantPoint != -1)
			{
				XPlyPoint* pt2 = (*iter).second[identifiantPoint];
				float p1 = pt2->Intensity();
				std::cout << "Point Trouve par correlation ! Numero : " << identifiantPoint << std::endl <<
					"Coordonnees : " << pt2->X() << " " << pt2->Y() << " " << pt2->Z() << std::endl <<
					"Intensite : " << p1 << " R : " << (float)pt2->R() << " B : " << (float)pt2->B() << " G : " << (float)pt2->G() << " ok" << std::endl;
				xr = pt2->X();
				yr = pt2->Y();
				zr = pt2->Z();
			}
		}
		catch (...) {
			XErrorError(m_error, __FUNCTION__, "La correlation a echouee");
		}

	}


	// R�cuperation des points de couleur dans le cas d'une sph�re
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

		// Test sur le nombre de pixels color�s
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
				x += p.X;
				y += p.Y;
				z += p.Z;
			}
			x_ = x / n;
			y_ = y / n;
			z_ = z / n;

			// Cordonn�es de la station
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

			// Ajout du rayon suivant le vecteur directeur
			float x_appr = x_ + 0.5*rayon*vecteur_directeur[0];
			float y_appr = y_ + 0.5*rayon*vecteur_directeur[1];
			float z_appr = z_ + 0.5*rayon*vecteur_directeur[2];

			int taille = (*iter).second.size();

			// Initialisation
			xr = x_appr;
			yr = y_appr;
			zr = z_appr;


			// Moindres carr�s
			// Y = AX
			// A
			Eigen::Matrix<double, Eigen::Dynamic, 3> A;
			A.resize(taille, 3);
			// Y
			Eigen::Matrix<double, Eigen::Dynamic, 1> Y;
			Y.resize(taille, 1);
			// P
			Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> P;
			P.resize(taille, taille);
			P.setIdentity();
			// X
			Eigen::Vector3d X;
			Eigen::Matrix3d B1;

			// Sigma
			float sigma = 1;
			float diff = 1;
			// Mesure d'it�ration
			int loop = 0;

			while (diff > 1e-15)
			{
				// Construction de A, Y et P
				for (int j = 0; j < (*iter).second.size(); j++)
				{
					XPlyPoint* pt = (*iter).second[j]; // Point j 
					XPt3D p = pt->Position();

					float d0 = sqrt(pow(p.X - xr, 2) + pow(p.Y - yr, 2) + pow(p.Z - zr, 2));
					A.row(j) << (xr - p.X) / d0, (yr - p.Y) / d0, (zr - p.Z) / d0;
					Y.row(j) << rayon - d0;
					if (abs(d0 - rayon) > 2 * rayon){
						P.row(j).col(j) << 0;
					}
				}

				// Construction de X
				B1 = ((A.transpose())*P)*A;
				X = ((B1.ldlt().solve((A.transpose()))*P)*Y);
				xr += X(0, 0);
				yr += X(1, 0);
				zr += X(2, 0);

				// Vecteur r�sidu
				Eigen::Matrix<double, Eigen::Dynamic, 1> V;
				V.resize(taille, 1);
				V = Y - A*X;

				// Facteur de variance
				float sigma2 = float(V.transpose()*P*V) / (taille - 3);
				diff = abs(sigma - sigma2);
				sigma = sigma2;

				// D�pond�ration
				for (int k = 0; k < taille; k++){
					if (abs(V(k, 0)) > 2 * sigma){
						P(k, k) /= 100;
					}
				}

				// Incr�menation
				loop += 1;

				// Si trop long
				if (loop > 30){
					XErrorInfo(m_error, __FUNCTION__, "It�ration trop importante -> conditions initiales erronn�es");
					break;
				}
			}
		}
		else
		{
			XErrorError(m_error, __FUNCTION__, "Pas suffisament de points");
		}
	}


	//Ecriture des fichiers de sorties
	if (xr!=0)
	{
		std::cout << std::endl << "..........................." <<std::endl << "Coordonnees du centre de la cible : " << std::endl <<
			xr << " " << yr << " " << zr << std::endl;

		float X = xr - (float)plyIn.m_Station.X;
		float Y = yr - (float)plyIn.m_Station.Y;
		float Z = zr - (float)plyIn.m_Station.Z;

		std::string nom = m_params.output_path + XPath::PathSep + fichier[1] + ".XYZ";
		std::ifstream fichierXYZ(nom.c_str(), std::ios::in);  // on ouvre le fichier en lecture

		if (fichierXYZ)  // si l'ouverture a r�ussi
		{
			bool etat = false;
			int nb_ligne = 0;
			// instructions
			// si elt d�j� �crit alors on sort
			std::string ligne;
			while (getline(fichierXYZ, ligne)){
				std::string::size_type trouve = ligne.find(fichier[0]);
				if ((float)trouve < 50){
					etat = true;
				}
				nb_ligne += 1;
			}
			fichierXYZ.seekg(0, std::ios::end);  //On se d�place � la fin du fichier		
			if (etat == true){
				fichierXYZ.close();
			}
			// sinon on va a la fin du fichier et on �crit
			else{
				std::ofstream fichierXYZ(nom.c_str(), std::ios::out | std::ios::app);
				fichierXYZ << nb_ligne << "  " << fichier[0] << "  " << X << "  " << Y << "  " << Z << std::endl;
				// on ferme le fichier
				fichierXYZ.close();
			}
		}
		else  // sinon
		{
			//cr�er un nouveau fichier 
			std::ofstream monflux(nom.c_str()); //d�claration du flux et ouverture du fichier 
			if (monflux)
			{
				//remplir l'ent�te
				monflux << "Station : " << fichier[1] << '(' << (float)plyIn.m_Station.X << ',' << (float)plyIn.m_Station.Y << ',' << (float)plyIn.m_Station.Z << ')' << std::endl;
				monflux << 1 << "  " << fichier[0] << "  " << X << "  " << Y << "  " << Z << std::endl;

				monflux.close();
			}
			else{
				XErrorError(m_error, __FUNCTION__, "Erreur dans l'�criture XYZ");
			}
		}
	}
	else
	{
		XErrorError(m_error, __FUNCTION__, "Non ecriture des coordonnees dans le systeme station");
	}
return true;	
}

