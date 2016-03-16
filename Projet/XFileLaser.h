#pragma once
#include "XError.h"
#include "XPt3D.h"

class XFileLaser;
class XLaserPoint : public XPt3D
{
public:
	std::string Id;
	TypeAppui type;
	float EctX;
	float EctY;
	float EctZ;
public:
	XLaserPoint();

};

class XFileLaser
{
public :
	XError* m_error;
	std::string m_strFilename;
	
	std::vector<XLaserPoint*> m_vecPoint;
	std::vector<std::string> m_vecCommentaire;//les commentaires sont mémorisés avec leur indice de point pour réécriture éventuelle
	std::vector<uint16> m_vecIndiceCommentaire;
	
	bool ProcessLineLaser(char* ligne);

public:
	XFileLaser(XError* error);
	~XFileLaser(void);
	
	void Clear();

	bool Load(std::string filename);
	std::string Filename() {return m_strFilename;}

	uint32 NbPoint(){return m_vecPoint.size();}
	XLaserPoint* Point(uint32 i){if(i>m_vecPoint.size())return NULL; return m_vecPoint[i]; }
};
