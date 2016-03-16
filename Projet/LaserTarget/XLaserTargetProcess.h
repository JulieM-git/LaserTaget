#pragma once
#include "XError.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XFileLaser.h"
#include <map>

//-----------------------------------------------------------------------------
class XLaserTargetParams
{
public:
	std::string input_path;
	std::string input_type;//extension des fichiers en entrée

	std::string output_path;
	std::string cor_inputfile;

public:
	XLaserTargetParams(void);
	~XLaserTargetParams(void){;}

	bool  CheckValid(XError* error);
	bool XmlWrite(std::ofstream* out);


};
class XPlyPoint;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class XLaserTargetProcess
{
private:
	XStringTools st;
	XPath P;
	XSystemInfo systm;

protected:
	XError* m_error;
	XFileLaser* m_Laser;

	static XLaserTargetParams m_params;		// Paramètres et Options de traitement
	static std::ofstream* out;
	static uint32 count;
	static bool ProcessPlyLaser3ply(XPlyPoint* pt);
	static std::map<XLaserPoint*, std::vector<XPlyPoint*>> vec3Ply;
	void InitLaserTarget();
	void ClearLaserTarget();

	std::vector<std::string> m_listFileToProcess;

	bool ProcessFile(std::string InputFilename);
	
public:
	XLaserTargetProcess(XError* error);
	~XLaserTargetProcess(void);

	bool LoadXmlFile(std::string filename);
	XLaserTargetParams* Params(){return &m_params;}	

	bool InitProcess();
	bool Process();

};
