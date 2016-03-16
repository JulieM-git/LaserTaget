#include "XFileTxtReader.h"

#include "XStringTools.h"
#include "XPath.h"
#include <fstream>
#include <algorithm>

//-----------------------------------------------------------------------------
bool XFileTxtReader::ReadLine(std::istream* in)
{
	in->getline(ligne,1023);
	if(!in->good())
		return false;

	return in->good();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool XFileTxtReader::Read(F_ProcessLine ProcessLine, F_ProcessLine ProcessComment)
{
	if(m_strFilename.empty())
		return XErrorError(m_error,__FUNCTION__,"nom de fichier vide");

	XStringTools ST;
	std::ifstream in;
	in.open(m_strFilename.c_str(), std::ios_base::in );
	if (in.fail())
		return XErrorError(m_error,__FUNCTION__, "Erreur ouverture fichier ", m_strFilename.c_str());

	char message[1024];
	NbLines =0;
	while(!in.eof())
	{
		if(!ReadLine(&in))
			break;
			 
		if(ST.IdentifyLineAsComment(std::string(ligne)))
		{
			if(ProcessComment != NULL)
				NbComments += ProcessComment(ligne);
			continue;
		}
		NbLines += ProcessLine(ligne);	
	}
	XPath P;
	sprintf(message,"%s : %d enregistrements valides + %d commentaires",P.Name(m_strFilename.c_str()).c_str(),NbLines,NbComments);
	return XErrorInfo(m_error,__FUNCTION__,message,"");;
}
