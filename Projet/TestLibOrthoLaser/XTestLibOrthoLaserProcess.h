#pragma once
#include "XError.h"
#include "XOrthoLaserChantier.h"

//-----------------------------------------------------------------------------
class XTestLibOrthoLaserParams
{
public:
	std::string input_path;
	std::string output_path;

	enum eTypPtsProcess {TestLibOrthoLaserProcess_Undef};
	eTypPtsProcess TypProcess;

public:
	XTestLibOrthoLaserParams(void){
		TypProcess = TestLibOrthoLaserProcess_Undef;
	}
	~XTestLibOrthoLaserParams(void){;}
	bool SetTypProcess(std::string name);
	std::string StrTypProcess();

	bool  CheckValid(XError* error);
	bool XmlWrite(std::ofstream* out);


};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class XTestLibOrthoLaserProcess
{
protected:
	XError* m_error;

	static XTestLibOrthoLaserParams m_params;		// Paramètres et Options de traitement
	static std::ofstream* out;
	static uint32 count;

	XOrthoLaserChantier* m_chantier;

	bool ProcessFile(std::string InputFilename);
	
public:
	XTestLibOrthoLaserProcess(XError* error);
	~XTestLibOrthoLaserProcess(void);

	bool LoadXmlFile(std::string filename);
	XTestLibOrthoLaserParams* Params(){return &m_params;}	

	std::string OutFileName(std::string InputFilename);

	bool InitProcess();
	bool Process();

};
