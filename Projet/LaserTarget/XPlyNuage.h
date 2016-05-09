#pragma once

#include "XBase.h"
#include "XError.h"
#include "XPt3D.h"

#include "XPlyPoint.h"

typedef bool (*F_PlyProcessPoint)(XPlyPoint* pt);
typedef bool (*F_PlyProcessLineEntete)(char* line);
//-----------------------------------------------------------------------------
class XPlyNuage
{
public:
	XPlyPoint* ptCourant;
	double StartTime;
	double EndTime;
	std::ofstream* out;
	XPt3D m_Station; //Coordonnées de la station (levés fixes terrestres)

protected:
	XError* m_error;
	std::string m_strFilename;
	std::ifstream in;
	//XPt3D m_Station; //Coordonnées de la station (levés fixes terrestres)

	XPt3D m_offset; //pour les coordonnées tronquées

	uint32 m_recordSize;//taille d'un enregistrement calculé lors de la lecture de l'entête
	uint32 m_nbVertex;//nombre d'éléments déclarés dans l'entête
	uint32 m_nNbPoint;//Nombre de points effectivement lus
	
	void Clear();
	bool WriteEntetePart1(std::ostream* out);

	bool AddProperty(std::string type, std::string attribut);
	bool AddComments(std::vector<std::string>& tokens);
	std::vector<std::string> m_vecProperty;
	std::vector<std::string> m_vecComment;

	bool ReadRGBFirst();//ordre de lecture des attributs

public:
	XPlyNuage(std::string filename, XError* error);//en lecture
	~XPlyNuage(void);

	bool InitOutput(XPlyPoint* ptCourant, uint32 nbVertex=0);//si on ne connait pas le nombre de points au départ il faudra revenir mettre a jour l'entete
	bool UpdateEnteteAndClose(uint32 nbVertex);
	bool CloseOutput();

	std::string Filename(){return m_strFilename;}

	XPt3D* Offset(){return &m_offset;}
	void Offset(XPt3D p ){m_offset = p;}

	XPt3D Station(){return m_Station;}
	void Station(XPt3D p ){m_Station = p;}
	
	void AddComment(std::string comment );
	uint32 NbVertex(){return m_nbVertex;}//nombre d'elements déclarés dans l'entete
	void NbVertex(uint32 nb){m_nbVertex = nb;}
	uint32 RecordSize(){return m_recordSize;}//taille d'un enregistrement calculé lors de la lecture de l'entête

	bool LoadTimes();
	bool WriteTimes(std::ostream* out);
	bool CheckTypePly(F_PlyProcessLineEntete ReadLine);

    bool ReadPly(F_PlyProcessPoint ProcessPoint, XPt3D Offset2 = XPt3D());
	bool ReadEntete(XPt3D Offset2 = XPt3D());
	bool ReadDatas(F_PlyProcessPoint ProcessPoint);

	bool WriteEntete(std::ostream* out,XPlyPoint* pointref= NULL);
	
	uint32 nbPoint(){return m_nNbPoint;}//nombre de pointeffectivement lus
	XPlyPoint* PointCourant(){return ptCourant;}

};
