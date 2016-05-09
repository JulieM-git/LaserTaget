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
	/*std::vector<std::string> type;
	std::vector<std::string> id;
	std::vector<std::string> nom;
	std::vector<std::string> rayon;
	std::vector<std::string> couleur;
	std::vector<std::string> image2D;
	std::vector<std::string> nuage;
	std::vector<std::string> resolution;*/

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


	static std::string cas;
	static std::string couleur;

	bool LoadXmlFile(std::string filename);
	XLaserTargetParams* Params(){return &m_params;}	
	XLaserTargetModel* Data(){ return &m_datas; }
	bool LoadXmlData(std::string filename);
	bool LoadCibleUSphere(std::string filename);


	bool InitProcess();
	bool Process();

};
