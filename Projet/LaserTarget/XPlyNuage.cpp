#include "XPlyNuage.h"
#include "XStringTools.h"
#include "XSystemInfo.h"
#include "XPath.h"

#include "XPlyPointDivers.h"
#include "XPlyPointPts.h"

#include <iomanip>

//-----------------------------------------------------------------------------
XPlyNuage::XPlyNuage(std::string filename, XError* error)
{
	m_strFilename = filename;
	m_error= error;
	m_nNbPoint =0;
	m_nbVertex= 0;
	ptCourant = NULL;
	m_recordSize =0;
	StartTime = 0;
	EndTime = 0;
	out = NULL;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::InitOutput(XPlyPoint* ptref, uint32 nbVertex)
{
	m_nbVertex = nbVertex;
	out = new std::ofstream(m_strFilename.c_str());
	WriteEntete(out,ptref);
	out->close();
	out->open(m_strFilename.c_str(), std::ios::binary | std::ios::app);
	return true;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::CloseOutput()
{
	if(out!=NULL)
		out->close();
	return true;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::WriteEntetePart1(std::ostream* out)
{
	*out << "ply" << '\n';
	*out << "format binary_little_endian 1.0" << '\n';

	int prec = out->precision(3);
	std::ios::fmtflags flags = out->setf(std::ios::fixed);

	if(m_offset != XPt3D())
		*out << "comment offset position " << m_offset.X << ' ' <<  m_offset.Y << ' ' << m_offset.Y << '\n';
	if(m_Station != XPt3D())
		*out << "comment station position " << m_Station.X << ' ' <<  m_Station.Y << ' ' << m_Station.Z << '\n';
	out->precision(prec);
	out->unsetf(std::ios::fixed);
	out->setf(flags);

	for(uint32 i=0; i< this->m_vecComment.size(); i++)
		*out << m_vecComment[i] <<  '\n';

	*out << "element vertex " <<std::setw(12)<<std::setfill('0')<< m_nbVertex << '\n';
	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyNuage::WriteEntete(std::ostream* out,XPlyPoint* pointref)
{
	WriteEntetePart1(out);
	if(pointref== NULL)
		pointref = ptCourant;
	pointref->WriteEntetePly(out);
	*out << "end_header"<< '\n';

	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyNuage::UpdateEnteteAndClose(uint32 nbVertex)
{    
	m_nbVertex = nbVertex;
	out->close();
	out->open(m_strFilename.c_str(),std::ios_base::in | std::ios_base::out);
    out->seekp(std::ios::beg);
	WriteEntetePart1(out);
	out->close();
	return true;
}
//-----------------------------------------------------------------------------
XPlyNuage::~XPlyNuage(void)
{
	Clear();
	if(out != NULL)
		out->close();
	delete out;
}
//-----------------------------------------------------------------------------
void XPlyNuage::Clear()
{
	if(ptCourant != NULL)
		delete ptCourant;
	ptCourant = NULL;
	m_offset = XPt3D();
	m_vecProperty.clear();
	m_nNbPoint = 0;
	m_recordSize =0;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::AddProperty(std::string type, std::string attribut)
{
	if(type.compare("float64") ==0)
	{
		m_recordSize +=8;
		m_vecProperty.push_back(attribut);
		return true;
	}
	if((type.compare("float") ==0)||(type.compare("float32") ==0))
	{
		m_recordSize +=4;
		m_vecProperty.push_back(attribut);
		return true;
	}
	if(type.compare("int") ==0)
	{
		m_recordSize +=sizeof(int);
		m_vecProperty.push_back(attribut);
		return true;
	}
	if(type.compare("uint16") ==0)
	{
		m_recordSize +=2;
		m_vecProperty.push_back(attribut);
		return true;
	}
	if((type.compare("uint8") ==0)||(type.compare("uchar") ==0))
	{
		m_recordSize +=1;
		m_vecProperty.push_back(attribut);
		return true;
	}	
	
	return XErrorError(m_error,__FUNCTION__,"Type de données non reconnue ",type.c_str());
}
//-----------------------------------------------------------------------------
bool XPlyNuage::AddComments(std::vector<std::string>& tokens)
{
	if((tokens.size() == 7)&&(tokens[3].compare("Pos") == 0))
	{//comment IGN offset Pos 649000.000000 6840000.000000 0.000000
		m_offset.X = atof(tokens[4].c_str());
		m_offset.Y = atof(tokens[5].c_str());
		m_offset.Z = atof(tokens[6].c_str());
		return false;
	}
	if((tokens.size() == 6)&&(tokens[0].compare("comment") == 0)&&(tokens[1].compare("station") == 0))
	{//comment station position 8600.39 8454.05 54.05
		m_Station.X = atof(tokens[3].c_str());
		m_Station.Y = atof(tokens[4].c_str());
		m_Station.Z = atof(tokens[5].c_str());
		return false;
	}	
	return true;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::ReadRGBFirst() // utilsé pour les plyPts pour gérer l'ordre de lecture de l'intensité avanat ou après les valeurs rgb
{
	for (uint32 i = 0; i < m_vecProperty.size(); i++)
	{
		if (m_vecProperty[i].compare("x") == 0)
			continue;
		if (m_vecProperty[i].compare("y") == 0)
			continue;
		if (m_vecProperty[i].compare("z") == 0)
			continue;
		if (m_vecProperty[i].compare("red") == 0)
			return true;
		//dans tous les autres cas
		return false;
	}
	return false;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::ReadEntete( XPt3D Offset2)
{
	in.open(m_strFilename.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!in.good())
		return XErrorError(m_error,__FUNCTION__, "Erreur ouverture fichier", m_strFilename.c_str());

	Clear();
	char ligne[1024];
	XStringTools ST;
	while(!in.eof())
	{
		in.getline(ligne,1023);
		if((in.eof()))
			return false;
		std::string lig = ligne;
		std::vector<std::string> tokens;
		ST.Tokenize(lig,tokens);
		if((tokens.size() == 3)&&(tokens[0].compare("property") == 0))
			AddProperty(tokens[1],tokens[2]);

		if((tokens[0].compare("comment") == 0)&&AddComments(tokens))
			m_vecComment.push_back(lig);

		if((tokens.size() == 3)&&(tokens[0].compare("element") == 0)&&(tokens[1].compare("vertex") == 0))
			m_nbVertex = atol(tokens[2].c_str());
		
		XStringTools st;
		if(tokens.size() == 1)
		{
			if(tokens[0] == "end_header")
				break;
			if(st.equal(tokens[0],std::string("end_header")))
				break;

		}
	}
	 
	m_offset.X += Offset2.X;//attention au signe de l'offset2 en entrée
	m_offset.Y += Offset2.Y;
	m_offset.Z += Offset2.Z;
	switch(m_recordSize)
	{
	case 80 :
		ptCourant = new XPlyPoint1(this);
		break;
	case 72 :
		ptCourant = new XPlyPoint2(this);
		break;
	case 86 :
		ptCourant = new XPlyPoint3(this);
		break;
	case 82 :
		ptCourant = new XPlyPoint3b(this);
		break;
	case 52 :
		ptCourant = new XPlyPoint4(this);
		break;
	case 19 :
		ptCourant = new XPlyPointPts(this,ReadRGBFirst());
		break;
	case 15 :
		ptCourant = new XPlyPoint5(this);
		break;
	case 16 :
		if(m_vecProperty.size() == 7)
			ptCourant = new XPlyPoint6(this);
		if(m_vecProperty.size() == 4)
			ptCourant = new XPlyPointSimple(this);
		break;
	default:
		char message[1024];
		sprintf(message," Taille=%d",m_recordSize);
		return XErrorError(m_error,__FUNCTION__,"Type d'enregistrement ply non reconnu ",message);
	}
	return true;
}

//-----------------------------------------------------------------------------
bool XPlyNuage::CheckTypePly(F_PlyProcessLineEntete ReadLine)
{
	std::ifstream in;
	in.open(m_strFilename.c_str(), std::ios_base::in);
	if (in.fail())
		return XErrorError(m_error,__FUNCTION__, "Erreur ouverture fichier", m_strFilename.c_str());
	Clear();
	char ligne[1024];
	XStringTools ST;
	while(!in.eof())
	{
		in.getline(ligne,1023);
		if((in.eof()))
			return false;
		ReadLine(ligne);
		std::string lig = ligne;
		std::vector<std::string> tokens;
		ST.Tokenize(lig,tokens);
		if((tokens.size() == 3)&&(tokens[0].compare("property") == 0))
			AddProperty(tokens[1],tokens[2]);

		if((tokens[0].compare("comment") == 0)&&AddComments(tokens))
			m_vecComment.push_back(lig);

		if((tokens.size() == 3)&&(tokens[0].compare("element") == 0)&&(tokens[1].compare("vertex") == 0))
			m_nbVertex = atol(tokens[2].c_str());
		
		if(lig.compare("end_header") == 0)
			break;
	}
	in.close();
	switch(m_recordSize)
	{
	case 80 :
		ptCourant = new XPlyPoint1(this);
		break;
	case 72 :
		ptCourant = new XPlyPoint2(this);
		break;
	case 82 :
		ptCourant = new XPlyPoint3b(this);
		break;
	case 86 :
		ptCourant = new XPlyPoint3(this);
		break;
	case 52 :
		ptCourant = new XPlyPoint4(this);
		break;
	case 19 :
		ptCourant = new XPlyPointPts(this,ReadRGBFirst());
		break;
	case 15 :
		ptCourant = new XPlyPoint5(this);
		break;
	case 16 :
		if(m_vecProperty.size() == 7)
			ptCourant = new XPlyPoint6(this);
		if(m_vecProperty.size() == 4)
			ptCourant = new XPlyPointSimple(this);
		break;
	default:
		char message[1024];
		sprintf(message," Taille=%d",m_recordSize);
		return XErrorError(m_error,__FUNCTION__,"Type d'enregistrement ply non reconnu ",message);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::ReadDatas(F_PlyProcessPoint ProcessPoint)
{
	char message[1024];
	uint32 count =0;
	while(!in.eof())
	{
		if(!ptCourant->Read(in))
			break;

		m_nNbPoint += ProcessPoint(ptCourant);
		if( count++ > m_nbVertex + 2) //problème de cohérence de l'entête ou pb de lecture
		{
			XErrorAlert(m_error,__FUNCTION__, "Problème de cohérence entre nb_vertex déclarés et lecture", m_strFilename.c_str());
			break;
		}
	}

	sprintf(message,"%d points lus : %d points utilises ",count,m_nNbPoint);
	return XErrorCommentaire(m_error,__FUNCTION__,message);
}
//-----------------------------------------------------------------------------
void XPlyNuage::AddComment(std::string comment )
{
	m_vecComment.push_back(std::string("comment ")+ comment);

}

//-----------------------------------------------------------------------------
bool XPlyNuage::ReadPly(F_PlyProcessPoint ProcessPoint, XPt3D Offset2)
{
	in.close();//au cas ou l'on ait fait un ReadEntete avant !
	in.clear();
	if(!ReadEntete(Offset2))
		return false;
	return ReadDatas(ProcessPoint);
}
//-----------------------------------------------------------------------------
bool XPlyNuage::LoadTimes()
{
	if(!ReadEntete())
		return false;

	char message[1024];
	if(!ptCourant->Read(in))
	{
		std::ostringstream oss;
		ptCourant->WriteTxt(&oss);
		sprintf(message,"Erreur lecture 1er point : %s",oss.str().c_str());
		return XErrorError(m_error,__FUNCTION__,message , m_strFilename.c_str());
	}
	StartTime = ptCourant->GPSTime();

	in.seekg((m_nbVertex-2)*m_recordSize,std::ios_base::cur);
	if(!ptCourant->Read(in))
	{
		std::ostringstream oss;
		ptCourant->WriteTxt(&oss);
		sprintf(message,"Erreur lecture dernier point : %s",oss.str().c_str());
		return XErrorError(m_error,__FUNCTION__, message, m_strFilename.c_str());
	}
	EndTime = ptCourant->GPSTime();
	in.close();	
	sprintf(message,"Heures GPS : premier point = %.2lf dernier point = %.2lf",StartTime,EndTime);
	return XErrorCommentaire(m_error,__FUNCTION__,message);;
}
//-----------------------------------------------------------------------------
bool XPlyNuage::WriteTimes(std::ostream* out)
{
	XPath p;
	*out << p.Name(m_strFilename.c_str()) << '\t' << StartTime << '\t' << EndTime << '\n';
	return out->good();
}
//-----------------------------------------------------------------------------