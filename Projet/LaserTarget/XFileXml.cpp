#include "XFileXml.h"
#include "XStringTools.h"
#include <fstream>

//-----------------------------------------------------------------------------
//********************* XFileXml *******************************************
//-----------------------------------------------------------------------------
XFileXml::XFileXml()
{
}


XFileXml::~XFileXml()
{
}

/*bool XFileXml::ProcessLineXml(char* ligne)
{
	XStringTools ST;
	std::vector<std::string> tokens;
	ST.Tokenize(ligne,tokens,std::string(" \t,"));
	if((tokens.size() < 7)||(tokens.size()> 8))
		return XErrorError(m_error,__FUNCTION__,"ligne COR non valide ", ligne);

	XLaserPoint* point = new XLaserPoint();

//0      1000       990.339       967.342       442.771   0.0010   0.0010   0.0001
	point->type = PXYZ;
	if (tokens[0].compare("3") == 0) 
		point->type = PZ;
	if (tokens[0].compare("2") == 0) 
		point->type = PXY;

	point->Id = tokens[1];
	point->X = atof(tokens[2].c_str());
	point->Y = atof(tokens[3].c_str());
	point->Z = atof(tokens[4].c_str());
	if(tokens.size()== 7)
	{
		point->EctX = atof(tokens[5].c_str());
		point->EctY = point->EctX;
		point->EctZ = atof(tokens[6].c_str());
	}
	if(tokens.size()== 8)
	{
		point->EctX = atof(tokens[5].c_str());
		point->EctY = atof(tokens[6].c_str());
		point->EctZ = atof(tokens[7].c_str());
	}
	m_vecPoint.push_back(point);
	return true;
}
//-----------------------------------------------------------------------------
bool XFileXml::Load(std::string filename)
{
	std::ifstream in;
	in.open(filename.c_str(), std::ios_base::in );
	if (in.fail())
		return XErrorError(m_error,__FUNCTION__, "Erreur ouverture ",filename.c_str());

	char ligne[1024];
	XStringTools st;
	while(!in.eof())
	{
		in.getline(ligne,1023);
		if(!in.good())
			break;
		if(st.IdentifyLineAsComment(std::string(ligne)))
		{
			this->m_vecCommentaire.push_back(std::string(ligne));
			this->m_vecIndiceCommentaire.push_back(this->m_vecPoint.size());
			continue;
		}
		ProcessLineXml(ligne);	
	}
	return true;
}
*/
