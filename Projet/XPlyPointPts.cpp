#include "XPlyPointPts.h"


//****************************  XPlyPointPts  **********************************
//-----------------------------------------------------------------------------
bool XPlyPointPts::Read(std::istream& in)
{
	uint16 test = sizeof(XPlyPointPtsrecord);
	if (this->m_bReadRgbFirst)
	{
		in.read((char*)&m_record, 12);
		in.read((char*)&m_record.R, 3);
		in.read((char*)&m_record.I, 4);
		return !in.eof();
	}
	in.read((char*)&m_record, 19);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPointPts::X(){	return m_record.x ;}
double XPlyPointPts::Y(){	return m_record.y ;}
double XPlyPointPts::Z(){	return m_record.z ;}
void  XPlyPointPts::X(double val){ m_record.x = (float)val;}
void  XPlyPointPts::Y(double val){ m_record.y = (float)val;}
void  XPlyPointPts::Z(double val){ m_record.z = (float)val;}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WriteEntetePly(std::ostream* out)
{
     *out<<"property float x"<<'\n';
    *out<<"property float y"<<'\n';
    *out<<"property float z"<<'\n';
    *out<<"property float scalar_Scalar_field"<<'\n';
    *out<<"property uchar red"<<'\n';
    *out<<"property uchar green"<<'\n';
    *out<<"property uchar blue"<<'\n';  
	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WriteEnteteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << "x"  << sep;
	*out << "y"  << sep;
	*out << "z"  << sep;
	*out << "I"  << sep;
	*out << "R"  << sep;
	*out << "G"  << sep;
	*out << "B" << '\n';
	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WriteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << m_record.x << sep;
	*out << m_record.y << sep;
	*out << m_record.z << sep;
	*out << m_record.I << sep;
	*out << (int) m_record.R << sep;
	*out << (int) m_record.G << sep;
	*out << (int) m_record.B << '\n';

	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WritePcd(std::ostream* out)
{
	return WritePcdTxt(out);
//	return WritePcdBin(out);
}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WritePcdTxt(std::ostream* out)
{
	char sep = ' ';
	//a voir : les couleurs sont mal déodées dans cloud compare
	float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	*out << m_record.x << sep <<  m_record.y << sep << m_record.z << sep << color << '\n';
	return out->good();
	
}
//-----------------------------------------------------------------------------
bool XPlyPointPts::WritePcdBin(std::ostream* out)
{
	float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	out->write((char*)&m_record,12);
	out->write((char*)&color,4);
	return out->good();
	
}
bool XPlyPointPts::WriteBinary(std::ofstream* out)
{
	//uint16 test = sizeof(XPlyPointPtsrecord);
	out->write((char*)&m_record, 19);
	//out->write((char*)&m_record, 12);
	//float fI = m_record.I;
	//out->write((char*)&fI, 4);
	//out->write((char*)&m_record.R, 1);
	//out->write((char*)&m_record.G, 1);
	//out->write((char*)&m_record.B, 1);
	return out->good();
}
XPlyPoint* XPlyPointPts::Clone()
{
	XPlyPointPts* newPoint = new XPlyPointPts(pparent,m_bReadRgbFirst);
	*newPoint = *this;
	return newPoint;
}