#include "XLaserTargetProcess.h"
#include "XArchiXMLException.h"
#include "XArchiXMLBaseTools.h"
#include "XArchiXMLTools.h"

#include "XSystemInfo.h"
#include "XPath.h"
#include "XPlyNuage.h"
#include "XPlanProcess.h"

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

	/*
	Eigen::MatrixXd m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	m(1, 1) = m(1, 0) + m(0, 1);
	std::cout << m << std::endl;
	*/
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

