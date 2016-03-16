#include "XTestLibOrthoLaserProcess.h"
#include "XErrorConsole.h"
#include "XSystemInfo.h"
#include "XPath.h"


#include <conio.h>

using namespace std;

XErrorConsoleTxt* m_error;
std::ofstream m_log;
std::string m_strAppPath;
XTestLibOrthoLaserProcess* m_process;


//-----------------------------------------------------------------------------
bool InitInternalData()
{
	XSystemInfo system;
	std::string logFile = system.CreateLogFile(&m_log,"txt");	
	if(logFile.size() == 0)
		return false;

	m_log.precision(3);	
	m_log.setf(std::ios::fixed);
	m_error = new XErrorConsoleTxt(&m_log);
	m_error->LogFileName(logFile);

	XErrorInfo(m_error,__FUNCTION__,"TestLibOrthoLaser version 0.1");

	// Recuperation du chemin de l'application
	m_strAppPath = system.GetExePath();
	XPath p;
	p.AddPathSep(m_strAppPath);

	m_process = new XTestLibOrthoLaserProcess(m_error);
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	std::cout<<"Initialisation interne " <<std::endl;
	if(!InitInternalData())
		return -1;

	std::string config = m_strAppPath + "TestLibOrthoLaser_Params.xml";
	if(argc > 1)
		config = m_strAppPath + argv[1];
	XErrorInfo(m_error,__FUNCTION__,"Chargement du fichier de config ",config.c_str());
	XSystemInfo system;
	if(!system.FindFile(config.c_str()))//on créefichier vide
	{
		XErrorAlert(m_error,__FUNCTION__,"Fichier de configuration non disponible");
		std::ofstream out(config.c_str());
		if (!out.good())
			return XErrorError(m_error,__FUNCTION__,"Erreur de creation de fichier",config.c_str());
		m_process->Params()->XmlWrite(&out);
		out.close();
		XErrorInfo(m_error,__FUNCTION__,"Le fichier de config (a renseigner) a ete cree",config.c_str());
		std::cout << "Taper sur une touche pour quitter le programme";
		int ch= _getch();
		return 0;
	}

	if(!m_process->LoadXmlFile(config))
		return -1;

	if( !m_process->InitProcess())
        return XErrorError(m_error,__FUNCTION__," Erreur au chargement du capteur");

	m_process->Process();

    std::cout<<" exit"<<std::endl;

  	return 0;
}
