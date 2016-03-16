#include "XTestLibOrthoLaserProcess.h"
#include "XArchiXMLException.h"
#include "XArchiXMLBaseTools.h"
#include "XArchiXMLTools.h"

#include "XSystemInfo.h"
#include "XPath.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserParams::SetTypProcess(std::string name)
{
	TypProcess = TestLibOrthoLaserProcess_Undef;
	if(name.compare("Undef") == 0)
	{
		TypProcess = TestLibOrthoLaserProcess_Undef;
		return true;
	}
	
	return false;
}
//-----------------------------------------------------------------------------
std::string XTestLibOrthoLaserParams::StrTypProcess()
{
	if(TypProcess == TestLibOrthoLaserProcess_Undef)
		return std::string("Undef");

	return std::string("Undef");
}

//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserParams::CheckValid(XError* error)
{
	XPath P;
	P.AddPathSep(input_path);
	P.AddPathSep(output_path);
	if(input_path == output_path)
		XErrorAlert(error,__FUNCTION__,"ATTENTION : Repertoires entree/sortie identiques ");
	return true;
}

//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserParams::XmlWrite(std::ofstream* out)
{
	*out << "<TestLibOrthoLaser_params>\n";

	*out << "<input_path>" << input_path << "</input_path>\n";
	*out << "<output_path>" << output_path << "</output_path>\n";

	*out << "</TestLibOrthoLaser_params>\n";

	return true;
}
//-------------------------------------------------------------------------
bool XTestLibOrthoLaserParams_LoadFileXml(XTestLibOrthoLaserParams* params,std::string filename)
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
	XArchiXML::AssertRoot(root,"TestLibOrthoLaser_params");

	params->input_path = XArchiXML::ReadAssertNodeAsString(root,std::string("input_path"));
	params->output_path = XArchiXML::ReadAssertNodeAsString(root,std::string("output_path"));
	//params->SetTypProcess(XArchiXML::ReadAssertNodeAsString(root,std::string("typ_process")));

	return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XTestLibOrthoLaserProcess::XTestLibOrthoLaserProcess(XError* error)
{
	m_error = error;
	m_chantier	= new XOrthoLaserChantier(m_error);

}
//-----------------------------------------------------------------------------
XTestLibOrthoLaserProcess::~XTestLibOrthoLaserProcess(void)
{
}
//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserProcess::LoadXmlFile(std::string filename)
{
	try
	{
		XTestLibOrthoLaserParams_LoadFileXml(&m_params,filename);
	}
	catch(XArchiXML::XmlException e)
	{
		return XErrorError(m_error,__FUNCTION__,e.Erreur().c_str(),filename.c_str());
	}

	return m_params.CheckValid(m_error);
}
//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserProcess::InitProcess()
{
	char message[1024];
	sprintf(message,"Repertoire en entree : %s",m_params.input_path.c_str());
	XErrorCommentaire(m_error,__FUNCTION__,message);
	m_chantier->AddFolderInput(m_params.input_path);

	sprintf(message,"Repertoire de sortie : %s",m_params.output_path.c_str());
	XErrorCommentaire(m_error,__FUNCTION__,message);

	XSystemInfo system;
	return system.CreateMultiDirectory(m_params.output_path.c_str());
}
//-----------------------------------------------------------------------------
//STATIC 
std::ofstream* XTestLibOrthoLaserProcess::out;
uint32 XTestLibOrthoLaserProcess::count;
XTestLibOrthoLaserParams XTestLibOrthoLaserProcess::m_params;


//-----------------------------------------------------------------------------
std::string XTestLibOrthoLaserProcess::OutFileName(std::string InputFilename)
{
	XPath P;
	std::string outfilename = P.Name(InputFilename.c_str());

	if(m_params.TypProcess == XTestLibOrthoLaserParams::TestLibOrthoLaserProcess_Undef)
	{
		outfilename = m_params.output_path + P.InsertBeforeExt(outfilename.c_str(),"_Undef");
	}
	return outfilename;
}

//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserProcess::Process()
{
	if (!m_chantier->LoadFolderOrthoLaser())
		return false; 

	if (!m_chantier->CreatePiles())//le frame est calculé avec le createPile
		return false;

	return XErrorInfo(m_error, __FUNCTION__, "Traitement ok");
}
//-----------------------------------------------------------------------------
bool XTestLibOrthoLaserProcess::ProcessFile(std::string filename)
{
	std::string outfile = OutFileName(filename);

	return true;	
}
