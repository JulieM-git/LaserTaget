#pragma once
#include "XError.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XFileLaser.h"
#include "XPlanProcess.h"
#include <map>
#include <vector>
#include <list>
#include <sys/stat.h>

//-----------------------------------------------------------------------------
class XLaserTargetParams
{
public:
	std::string input_path;
	std::string input_type;//extension des fichiers en entrée

	std::string output_path;
	std::string txt_inputfile;

	std::string cor_inputfile;

public:
	XLaserTargetParams(void);
	~XLaserTargetParams(void){;}

	bool  CheckValid(XError* error);
	bool XmlWrite(std::ofstream* out);


};
//-----------------------------------------------------------------------------
class model{
public:
	std::string type;
	int id;
	std::string nom;
	float rayon;
	std::string couleur;
	std::string image2D;
	std::string nuage;
	float resolution;
};

class XLaserTargetModel
{
public:
	std::list<model> model_list;
	std::string data_path;

public:
	XLaserTargetModel(void);
	~XLaserTargetModel(void){ ; }

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
	static XLaserTargetModel m_datas;
	static std::ofstream* out;
	static uint32 count;
	static bool ProcessPlyLaser3ply(XPlyPoint* pt);
	static std::map<XLaserPoint*, std::vector<XPlyPoint*>> vec3Ply;
	void InitLaserTarget();
	void ClearLaserTarget();

	static std::vector<std::string> liste_txt_nom;
	static std::vector<int> liste_txt_id;
	
	std::vector<std::string> m_listFileToProcess;

	bool ProcessFile(std::string InputFilename);
	
public:
	XLaserTargetProcess(XError* error);
	~XLaserTargetProcess(void);

	inline bool  exists_test(const std::string& name);
	static std::string cas;
	static std::string couleur;
	static std::string image2D;
	static float resolution;
	static float rayon;

	bool LoadXmlFile(std::string filename);
	XLaserTargetParams* Params(){return &m_params;}	
	XLaserTargetModel* Data(){ return &m_datas; }
	bool LoadXmlData(std::string filename);
	bool LoadCibleUSphere(std::string filename);


	bool InitProcess();
	bool Process();

};
