#include "XPlyPointDivers.h"

#include "XPlyPoint.h"
#include "XPlyNuage.h"

//-----------------------------------------------------------------------------
//***********  XPlyPoint1 XPlyPoint2 XPlyPoint3 ******************************
//-----------------------------------------------------------------------------
bool XPlyPoint1::Read(std::istream& in)
{
	in.read((char*)&m_record,sizeRecord());
	return !in.eof();
} 
bool XPlyPoint1::WriteEntetePly(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
bool XPlyPoint1::WriteEnteteTxt(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
bool XPlyPoint1::WriteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
bool XPlyPoint1::WritePcd(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
	
}
bool XPlyPoint1::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,sizeRecord());
	return out->good();
}

XPlyPoint* XPlyPoint1::Clone()
{
	XPlyPoint1* newPoint = new XPlyPoint1(pparent);
	*newPoint = *this;
	return newPoint;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool XPlyPoint2::Read(std::istream& in)
{
	in.read((char*)&m_record,sizeRecord());
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPoint2::X(){return m_record.x + pparent->Offset()->X;}
//-----------------------------------------------------------------------------
double XPlyPoint2::Y(){return m_record.y + pparent->Offset()->Y;}
//-----------------------------------------------------------------------------
double XPlyPoint2::Z(){return m_record.z + pparent->Offset()->Z;}
//-----------------------------------------------------------------------------
bool XPlyPoint2::WriteEntetePly(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
bool XPlyPoint2::WriteEnteteTxt(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
bool XPlyPoint2::WriteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
bool XPlyPoint2::WritePcd(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
	
}
bool XPlyPoint2::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,sizeRecord());
	return out->good();
}
XPlyPoint* XPlyPoint2::Clone()
{
	XPlyPoint2* newPoint = new XPlyPoint2(pparent);
	*newPoint = *this;
	return newPoint;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool XPlyPoint3::Read(std::istream& in)
{
	in.read((char*)&m_record,sizeRecord()-16);
	in.read((char*)&m_record.amplitude,16);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPoint3::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPoint3::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPoint3::Z(){	return m_record.z + pparent->Offset()->Z;}
//-----------------------------------------------------------------------------
bool XPlyPoint3::WriteEntetePly(std::ostream* out)
{
    *out<<"property float64 GPS_time"<<'\n';
    *out<<"property float32 x_sensor"<<'\n';
    *out<<"property float32 y_sensor"<<'\n';
    *out<<"property float32 z_sensor"<<'\n';
    *out<<"property float32 x_origin_sensor"<<'\n';
    *out<<"property float32 y_origin_sensor"<<'\n';
    *out<<"property float32 z_origin_sensor"<<'\n';
    *out<<"property float32 x"<<'\n';
    *out<<"property float32 y"<<'\n';
    *out<<"property float32 z"<<'\n';
    *out<<"property float32 x_origin"<<'\n';
    *out<<"property float32 y_origin"<<'\n';
    *out<<"property float32 z_origin"<<'\n';
    *out<<"property float32 range"<<'\n';
    *out<<"property float32 theta"<<'\n';
    *out<<"property float32 phi"<<'\n';
    *out<<"property uint8 num_echo"<<'\n';
    *out<<"property uint8 nb_of_echo"<<'\n';
    *out<<"property float32 amplitude"<<'\n';
    *out<<"property float32 reflectance"<<'\n';
    *out<<"property float32 deviation"<<'\n';
    *out<<"property float32 background_radiation"<<'\n';	
	return out->good();
}
bool XPlyPoint3::WriteEnteteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPoint3::WriteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
bool XPlyPoint3::WritePcd(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
	
}
bool XPlyPoint3::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,sizeRecord()-16);
	out->write((char*)&m_record.amplitude,16);
	return out->good();
}
XPlyPoint* XPlyPoint3::Clone()
{
	XPlyPoint3* newPoint = new XPlyPoint3(pparent);
	*newPoint = *this;
	return newPoint;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool XPlyPoint3b::Read(std::istream& in)
{
	in.read((char*)&m_record,sizeRecord()-12);
	in.read((char*)&m_record.amplitude,12);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPoint3b::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPoint3b::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPoint3b::Z(){	return m_record.z + pparent->Offset()->Z;}
//-----------------------------------------------------------------------------
XPt3D XPlyPoint3b::Origine()
{
	return XPt3D(m_record.x_origin + + pparent->Offset()->X,
		m_record.y_origin + + pparent->Offset()->Y,
		m_record.z_origin + + pparent->Offset()->Z);
}
//-----------------------------------------------------------------------------
bool XPlyPoint3b::WriteEntetePly(std::ostream* out)
{
    *out<<"property float64 time"<<'\n';
    *out<<"property float32 x_sensor"<<'\n';
    *out<<"property float32 y_sensor"<<'\n';
    *out<<"property float32 z_sensor"<<'\n';
    *out<<"property float32 x_origin_sensor"<<'\n';
    *out<<"property float32 y_origin_sensor"<<'\n';
    *out<<"property float32 z_origin_sensor"<<'\n';
    *out<<"property float32 x"<<'\n';
    *out<<"property float32 y"<<'\n';
    *out<<"property float32 z"<<'\n';
    *out<<"property float32 x_origin"<<'\n';
    *out<<"property float32 y_origin"<<'\n';
    *out<<"property float32 z_origin"<<'\n';
    *out<<"property float32 range"<<'\n';
    *out<<"property float32 theta"<<'\n';
    *out<<"property float32 phi"<<'\n';
    *out<<"property uint8 num_echo"<<'\n';
    *out<<"property uint8 nb_of_echo"<<'\n';
    *out<<"property float32 amplitude"<<'\n';
    *out<<"property float32 reflectance"<<'\n';
    *out<<"property float32 deviation"<<'\n';
	return out->good();
}
bool XPlyPoint3b::WriteEnteteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
bool XPlyPoint3b::WriteTxt(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
bool XPlyPoint3b::WritePcd(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
	
}
bool XPlyPoint3b::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,sizeRecord()-12);
	out->write((char*)&m_record.amplitude,12);
	return out->good();
}
XPlyPoint* XPlyPoint3b::Clone()
{
	XPlyPoint3b* newPoint = new XPlyPoint3b(pparent);
	*newPoint = *this;
	return newPoint;
}
//-----------------------------------------------------------------------------
//Ply velodytne
//-----------------------------------------------------------------------------
bool XPlyPoint4::Read(std::istream& in)
{
	uint16 test = sizeof(XPlyPoint4record);
	in.read((char*)&m_record,11);
	in.read((char*)&m_record.theta,4);
	in.read((char*)&m_record.fiber,1);
	in.read((char*)&m_record.x_laser,36);
	return !in.eof();
}
//-----------------------------------------------------------------------------
bool XPlyPoint4::WriteEntetePly(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPoint4::WriteEnteteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << "GPS_time" << sep;
	*out << "range" << sep;
	*out << "intensity" << sep;
	*out << "theta" << sep;
	*out << "block_id"  << sep;
	*out << "fiber" << sep;
	*out << "x_laser" << sep;
	*out << "y_laser" << sep;
	*out << "z_laser" << sep;
	*out << "x"  << sep;
	*out << "y"  << sep;
	*out << "z"  << sep;
	*out <<"x_centre_laser" << sep;
	*out << "y_centre_laser" << sep;
	*out << "z_centre_laser" << '\n';
	return out->good();
}
//-----------------------------------------------------------------------------
double XPlyPoint4::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPoint4::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPoint4::Z(){	return m_record.z + pparent->Offset()->Z;}
//-----------------------------------------------------------------------------
bool XPlyPoint4::WriteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << m_record.GPS_time << sep;
	*out << m_record.range << sep;
	*out << (int) m_record.intensity << sep;
	*out << m_record.theta << sep;
	*out << m_record.block_id  << sep;
	*out << (int) m_record.fiber << sep;
	*out << m_record.x_laser << sep;
	*out << m_record.y_laser << sep;
	*out << m_record.z_laser << sep;
	*out << m_record.x  << sep;
	*out << m_record.y  << sep;
	*out << m_record.z  << sep;
	*out << m_record.x_centre_laser << sep;
	*out << m_record.y_centre_laser << sep;
	*out << m_record.z_centre_laser << '\n';

	return out->good();
}
//-----------------------------------------------------------------------------
bool XPlyPoint4::WritePcd(std::ostream* out)
{
	*out << "non implémenté\n";
	return out->good();
	
}
//-----------------------------------------------------------------------------
bool XPlyPoint4::WriteBinary(std::ofstream* out)
{
	*out << "non implémenté\n";
	return out->good();
}
XPlyPoint* XPlyPoint4::Clone()
{
	XPlyPoint4* newPoint = new XPlyPoint4(pparent);
	*newPoint = *this;
	return newPoint;
}
//-----------------------------------------------------------------------------
double XPlyPoint4::GPSTime()
{
	double test;
	double jour = 3600*24.;
	double reste = modf(m_record.GPS_time/jour,&test);

	return reste*jour;
}

//-----------------------------------------------------------------------------
//Ply micmac
//-----------------------------------------------------------------------------
bool XPlyPoint5::Read(std::istream& in)
{
	uint16 test = sizeof(XPlyPoint5record);
	in.read((char*)&m_record,12);
	in.read((char*)&m_record.R,3);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPoint5::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPoint5::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPoint5::Z(){	return m_record.z + pparent->Offset()->Z;}
void  XPlyPoint5::X(double val){ m_record.x = (float)(val - pparent->Offset()->X);}
void  XPlyPoint5::Y(double val){ m_record.y = (float)(val - pparent->Offset()->Y);}
void  XPlyPoint5::Z(double val){ m_record.z = (float)(val - pparent->Offset()->Z);}
//-----------------------------------------------------------------------------
bool XPlyPoint5::WriteEntetePly(std::ostream* out)
{
    *out<<"property float32 x"<<'\n';
    *out<<"property float32 y"<<'\n';
    *out<<"property float32 z"<<'\n';
    *out<<"property uchar red"<<'\n';
    *out<<"property uchar green"<<'\n';
    *out<<"property uchar blue"<<'\n';  
	return out->good();
}
bool XPlyPoint5::WriteEnteteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << "x"  << sep;
	*out << "y"  << sep;
	*out << "z"  << sep;
	*out << "R"  << sep;
	*out << "G"  << sep;
	*out << "B" << '\n';
	return out->good();
}
bool XPlyPoint5::WriteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << m_record.x << sep;
	*out << m_record.y << sep;
	*out << m_record.z << sep;
	*out << (int) m_record.R << sep;
	*out << (int) m_record.G << sep;
	*out << (int) m_record.B << '\n';

	return out->good();
}
bool XPlyPoint5::WritePcd(std::ostream* out)
{
	return WritePcdTxt(out);
//	return WritePcdBin(out);
}
bool XPlyPoint5::WritePcdTxt(std::ostream* out)
{
	char sep = ' ';
	float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	*out << m_record.x << sep <<  m_record.y << sep << m_record.z << sep << color << '\n';
	return out->good();
	
}
bool XPlyPoint5::WritePcdBin(std::ostream* out)
{
	float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	out->write((char*)&m_record,12);
	out->write((char*)&color,4);
	return out->good();
	
}bool XPlyPoint5::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,15);
	return out->good();
}
XPlyPoint* XPlyPoint5::Clone()
{
	XPlyPoint5* newPoint = new XPlyPoint5(pparent);
	*newPoint = *this;
	return newPoint;
}

//-----------------------------------------------------------------------------
//Ply micmac + alpha
//-----------------------------------------------------------------------------
bool XPlyPoint6::Read(std::istream& in)
{
	in.read((char*)&m_record,12);
	in.read((char*)&m_record.R,4);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPoint6::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPoint6::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPoint6::Z(){	return m_record.z + pparent->Offset()->Z;}
void  XPlyPoint6::X(double val){ m_record.x = (float)(val - pparent->Offset()->X);}
void  XPlyPoint6::Y(double val){ m_record.y = (float)(val - pparent->Offset()->Y);}
void  XPlyPoint6::Z(double val){ m_record.z = (float)(val - pparent->Offset()->Z);}
//-----------------------------------------------------------------------------
bool XPlyPoint6::WriteEntetePly(std::ostream* out)
{
	*out << "non implémenté";
	return out->good();
}
bool XPlyPoint6::WriteEnteteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << "x"  << sep;
	*out << "y"  << sep;
	*out << "z"  << sep;
	*out << "R"  << sep;
	*out << "G"  << sep;
	*out << "B" << sep;
	*out << "alpha" << '\n';
	return out->good();
}
bool XPlyPoint6::WriteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << m_record.x << sep;
	*out << m_record.y << sep;
	*out << m_record.z << sep;
	*out << (int) m_record.R << sep;
	*out << (int) m_record.G << sep;
	*out << (int) m_record.B << '\n';
	*out << (int) m_record.alpha << '\n';

	return out->good();
}
bool XPlyPoint6::WritePcd(std::ostream* out)
{
	char sep = ' ';
	float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	*out << m_record.x << sep <<  m_record.y << sep << m_record.z << sep << color << '\n';
	return out->good();
	
}
bool XPlyPoint6::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,16);
	return out->good();
}
XPlyPoint* XPlyPoint6::Clone()
{
	XPlyPoint6* newPoint = new XPlyPoint6(pparent);
	*newPoint = *this;
	return newPoint;
}
//-----------------------------------------------------------------------------
//Ply simple
//-----------------------------------------------------------------------------
bool XPlyPointSimple::Read(std::istream& in)
{
	in.read((char*)&m_record,16);
	return !in.eof();
}
//-----------------------------------------------------------------------------
double XPlyPointSimple::X(){	return m_record.x + pparent->Offset()->X;}
double XPlyPointSimple::Y(){	return m_record.y + pparent->Offset()->Y;}
double XPlyPointSimple::Z(){	return m_record.z + pparent->Offset()->Z;}
float XPlyPointSimple::Intensity(){	return m_record.intensite;}
void  XPlyPointSimple::X(double val){ m_record.x = (float)(val - pparent->Offset()->X);}
void  XPlyPointSimple::Y(double val){ m_record.y = (float)(val - pparent->Offset()->Y);}
void  XPlyPointSimple::Z(double val){ m_record.z = (float)(val - pparent->Offset()->Z);}
void XPlyPointSimple::Intensity(float val){ m_record.intensite = val; }

//-----------------------------------------------------------------------------
bool XPlyPointSimple::WriteEntetePly(std::ostream* out)
{
    *out<<"property float32 x"<<'\n';
    *out<<"property float32 y"<<'\n';
    *out<<"property float32 z"<<'\n';
    *out<<"property float32 scalar_Scalar_field"<<'\n';
	return out->good();
}
bool XPlyPointSimple::WriteEnteteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << "x"  << sep;
	*out << "y"  << sep;
	*out << "z"  << sep;
	*out << "intensite" << '\n';
	return out->good();
}
bool XPlyPointSimple::WriteTxt(std::ostream* out)
{
	char sep = '\t';
	*out << m_record.x << sep;
	*out << m_record.y << sep;
	*out << m_record.z << sep;
	*out << m_record.intensite << '\n';

	return out->good();
}
bool XPlyPointSimple::WritePcd(std::ostream* out)
{
	char sep = ' ';
	//float color = m_record.R*65536.+ m_record.G*256.+m_record.B;
	float color = 0; //a voir;
	*out << m_record.x << sep <<  m_record.y << sep << m_record.z << sep << color << '\n';
	return out->good();
	
}
bool XPlyPointSimple::WriteBinary(std::ofstream* out)
{
	out->write((char*)&m_record,16);
	return out->good();
}
XPlyPoint* XPlyPointSimple::Clone()
{
	XPlyPointSimple* newPoint = new XPlyPointSimple(pparent);
	*newPoint = *this;
	return newPoint;
}

