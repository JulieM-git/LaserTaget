#pragma once

#include "XBase.h"
#include "XPt3D.h"
#include "XPlyPoint.h"

//-----------------------------------------------------------------------------
struct XPlyPoint2record
{
    double GPS_time;
    float x_coord_ref_laser;
    float y_coord_ref_laser;
    float z_coord_ref_laser;
    float x_ref_laser_origine;
    float y_ref_laser_origine;
    float z_ref_laser_origine;
    float x;
    float y;
    float z;
    double echo_range;
    uint16 theta;
    byte num_echo;
    byte nb_of_echo;
    float amplitude;
    float reflectance;
    float deviation;
    float background_radiation;
};
//-----------------------------------------------------------------------------
class XPlyPoint2 : public XPlyPoint //attention les attributs de petites tailles sont a la fin sinon sizeof(XPlyPoint) renvoie 80 au lieu de 72
{
protected:
	XPlyPoint2record m_record;
public:
	XPlyPoint2(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 72;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void X(double val){m_record.x = val;}
	virtual void Y(double val){m_record.y = val;}
	virtual void Z(double val){m_record.z = val;}
	virtual double GPSTime(){return m_record.GPS_time;}
	virtual float Intensity(){return m_record.amplitude;}
	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPoint1record
{
	double GPS_time;
	float R;
	float theta;
	float intensite;
	double E_pt;
	double N_pt;
	double h_pt;
	float std_E;
	float std_N;
	float std_h;
	double E_laser;
	double N_laser;
	double h_laser;
};
//-----------------------------------------------------------------------------
class XPlyPoint1 : public XPlyPoint
{
public:
	XPlyPoint1record m_record;
public:
	XPlyPoint1(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 80;}
	virtual double X(){return m_record.E_pt;}
	virtual double Y(){return m_record.N_pt;}
	virtual double Z(){return m_record.h_pt;}
	virtual void X(double val){m_record.E_pt = val;}
	virtual void Y(double val){m_record.N_pt = val;}
	virtual void Z(double val){m_record.h_pt = val;}
	virtual double GPSTime(){return m_record.GPS_time;}
	virtual float Intensity(){return m_record.intensite;}
	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPoint3record
{
	double GPS_time;
	float x_sensor;
	float y_sensor;
	float z_sensor;
	float x_origin_sensor;
	float y_origin_sensor;
	float z_origin_sensor;
	float x ;
	float y ;
	float z ;
	float x_origin;
	float y_origin;
	float z_origin;
	float range;
	float theta;
	float phi;
	unsigned char num_echo;
	unsigned char nb_of_echo;
	float amplitude;
	float reflectance;
	float deviation;
	float background_radiation;
};
//-----------------------------------------------------------------------------
struct XPlyPoint3brecord
{
	double GPS_time;
	float x_sensor;
	float y_sensor;
	float z_sensor;
	float x_origin_sensor;
	float y_origin_sensor;
	float z_origin_sensor;
	float x ;
	float y ;
	float z ;
	float x_origin;
	float y_origin;
	float z_origin;
	float range;
	float theta;
	float phi;
	unsigned char num_echo;
	unsigned char nb_of_echo;
	float amplitude;
	float reflectance;
	float deviation;
};
//-----------------------------------------------------------------------------
class XPlyPoint3 : public XPlyPoint // Ply  old
{
public:
	XPlyPoint3record m_record;
public:
	XPlyPoint3(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 86;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void X(double val){m_record.x = val;}
	virtual void Y(double val){m_record.y = val;}
	virtual void Z(double val){m_record.z = val;}

	virtual double GPSTime(){return m_record.GPS_time;}
	virtual float Intensity(){return m_record.amplitude;}
	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
class XPlyPoint3b : public XPlyPoint // Ply Stéréopolis Riegl
{
public:
	XPlyPoint3brecord m_record;
public:
	XPlyPoint3b(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 82;}

	virtual double X();
	virtual double Y();
	virtual double Z();
	//****TODO a modifier idem cf XPlyPoint5 et tester dans rieglexport
	virtual void X(double val){m_record.x = val;}
	virtual void Y(double val){m_record.y = val;}
	virtual void Z(double val){m_record.z = val;}
	XPt3D Origine();

	virtual double GPSTime(){return m_record.GPS_time;}
	//a voir amplitude ou reflectance
	//virtual float Intensity(){return m_record.amplitude;}
	virtual float Intensity(){return m_record.reflectance;}
	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPoint4record
{
	double GPS_time;
	uint16 range ;
	byte intensity ;
	uint16 theta ;
	uint16 block_id ;
	byte fiber ;
	float x_laser;
	float y_laser;
	float z_laser;
	float x ;
	float y ;
	float z ;
	float x_centre_laser;
	float y_centre_laser;
	float z_centre_laser;
};
//-----------------------------------------------------------------------------
class XPlyPoint4 : public XPlyPoint //Ply Velodyne
{
public:
	XPlyPoint4record m_record;
public:
	XPlyPoint4(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 52;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void X(double val){m_record.x = val;}
	virtual void Y(double val){m_record.y = val;}
	virtual void Z(double val){m_record.z = val;}
	virtual double GPSTime();
	virtual float Intensity(){return m_record.intensity;}
	virtual bool Read(std::istream& in);
	virtual bool IsTyp_H_Velodyne(){return true;}
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPoint5record //Ply micmac
{
	float x ;
	float y ;
	float z ;
	byte R ;
	byte G ;
	byte B ;

};
//-----------------------------------------------------------------------------
class XPlyPoint5 : public XPlyPoint //Ply micmac
{
public:
	XPlyPoint5record m_record;

	bool WritePcdTxt(std::ostream* out);
	bool WritePcdBin(std::ostream* out);

public:
	XPlyPoint5(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 15;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void  X(double val);
	virtual void  Y(double val);
	virtual void  Z(double val);

	virtual byte R(){return m_record.R;}
	virtual byte G(){return m_record.G;}
	virtual byte B(){return m_record.B;}
	virtual void R(byte val){m_record.R = val;}
	virtual void G(byte val){m_record.G = val;}
	virtual void B(byte val){m_record.B = val;}
	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual float Intensity(){return (m_record.R +m_record.G+m_record.B)/3;}
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPoint6record
{
	float x ;
	float y ;
	float z ;
	byte R ;
	byte G ;
	byte B ;
	byte alpha ;

};
//-----------------------------------------------------------------------------
class XPlyPoint6 : public XPlyPoint //Ply micmac + alpha
{
public:
	XPlyPoint6record m_record;
public:
	XPlyPoint6(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 16;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void  X(double val);
	virtual void  Y(double val);
	virtual void  Z(double val);

	virtual byte R(){return m_record.R;}
	virtual byte G(){return m_record.G;}
	virtual byte B(){return m_record.B;}
	virtual byte alpha(){return m_record.alpha;}

	virtual void R(byte val){m_record.R = val;}
	virtual void G(byte val){m_record.G = val;}
	virtual void B(byte val){m_record.B = val;}
	virtual void alpha(byte val){m_record.alpha = val;}

	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
//-----------------------------------------------------------------------------
struct XPlyPointSimplerecord
{
	float x ;
	float y ;
	float z ;
	float intensite ;

};
//-----------------------------------------------------------------------------
class XPlyPointSimple : public XPlyPoint //Ply simple
{
public:
	XPlyPointSimplerecord m_record;
public:
	XPlyPointSimple(XPlyNuage* parent) : XPlyPoint(parent){;}

	virtual uint16 sizeRecord(){return 16;}
	virtual double X();
	virtual double Y();
	virtual double Z();
	virtual void  X(double val);
	virtual void  Y(double val);
	virtual void  Z(double val);
	virtual float Intensity();
	void Intensity(float val);


	virtual bool Read(std::istream& in);
	virtual bool WriteEntetePly(std::ostream* out);
	virtual bool WriteEnteteTxt(std::ostream* out);
	virtual bool WriteTxt(std::ostream* out);
	virtual bool WritePcd(std::ostream* out);
	virtual bool WriteBinary(std::ofstream* out);
	virtual XPlyPoint* Clone();
};
