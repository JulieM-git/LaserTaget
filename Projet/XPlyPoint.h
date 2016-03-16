#ifndef _XPLYPOINT_H
#define _XPLYPOINT_H

#include "XPt3D.h"
#include <sstream>

class XPlyNuage;
class XPlyPoint
{
public:
	XPlyNuage* pparent;
public:
	XPlyPoint(XPlyNuage* parent);
	~XPlyPoint(){;}
	virtual uint16 sizeRecord() =0;

	virtual double X()=0;
	virtual double Y()=0;
	virtual double Z()=0;
	XPt3D Position(){return XPt3D(X(),Y(),Z());}
	XPt3D PositionOriginale();

	virtual void X(double val)=0;
	virtual void Y(double val)=0;
	virtual void Z(double val)=0;

	virtual double GPSTime(){return 0.;}
	virtual float Intensity(){return 0;}
	virtual byte R(){return 0;}
	virtual byte G(){return 0;}
	virtual byte B(){return 0;}

	virtual bool Read(std::istream& in)=0;
	virtual bool WriteEntetePly(std::ostream* out)=0;
	virtual bool WriteEnteteTxt(std::ostream* out)=0;
	virtual bool WriteTxt(std::ostream* out)=0;
	virtual bool WritePcd(std::ostream* out)=0;
	virtual bool WriteBinary(std::ofstream* out)=0;
	virtual bool IsTyp_H_Velodyne(){return false;}
	virtual XPlyPoint* Clone()=0;

};
#endif //_XPLYPOINT_H