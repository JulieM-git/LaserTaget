#include "XLaserTargetProcess.h"
#include "XErrorConsole.h"
#include "XSystemInfo.h"
#include "XPath.h"
#include "XImageTiffLoader.h"
#include "XRawImage.h"

#include <conio.h>
#include <time.h>

using namespace std;

XErrorConsoleTxt* m_error;
std::ofstream m_log;
std::string m_strAppPath;
XLaserTargetProcess* m_process;


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

	XErrorInfo(m_error,__FUNCTION__,"LaserTarget version 0.1");

	// Recuperation du chemin de l'application
	m_strAppPath = system.GetExePath();
	XPath p;
	p.AddPathSep(m_strAppPath);
	
	m_process= new XLaserTargetProcess(m_error);
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{

	std::cout<<"Initialisation interne " <<std::endl;
	if(!InitInternalData())
		return -1;

	std::string config = m_strAppPath + "LaserTarget_Params.xml";
	if(argc > 1)
		config = m_strAppPath + argv[1];
	XErrorInfo(m_error,__FUNCTION__,"Chargement du fichier de config ",config.c_str());
	XSystemInfo system;
	if(!system.FindFile(config.c_str()))//on crée un fichier vide
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
    
	std::string data = m_strAppPath + "LaserTarget_Model1.xml";
	if (argc > 1)
		data = m_strAppPath + argv[1];
	XErrorInfo(m_error, __FUNCTION__, "Chargement du fichier de model ", data.c_str());
	//XSystemInfo system;
	if (!system.FindFile(data.c_str()))
	{
		XErrorError(m_error, __FUNCTION__, "Erreur: Fichier model introuvable");
		std::cout << "Taper sur une touche pour quitter le programme";
		int ch = _getch();
		return 0;
	}

	if (!m_process->LoadXmlData(data))
		return -1;

	//if (!m_process->InitProcess())
		//return XErrorError(m_error, __FUNCTION__, " Erreur au chargement du capteur");
      
	  
	XImageTiffWriter*  TIFFWriter = new XImageTiffWriter(m_error);
	XRawImage::RegisterWriter(TIFFWriter, "tif");

	m_process->Process();

    std::cout<<" exit"<<std::endl;

  	return 0;
}
