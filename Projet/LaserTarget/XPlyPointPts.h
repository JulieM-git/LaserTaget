#ifndef _XPLYPOINTPTS_H
#define _XPLYPOINTPTS_H

#include "XBase.h"

#include "XPlyPoint.h"
#include "XPlyNuage.h"

struct XPlyPointPtsrecord
{
	float x ;
	float y ;
	float z ;
	float I;
	byte R ;
	byte G ;
	byte B ;

};
//-----------------------------------------------------------------------------
class XPlyPointPts: public XPlyPoint 
{
public:
	XPlyPointPtsrecord m_record;

	bool WritePcdTxt(std::ostream* out);
	bool WritePcdBin(std::ostream* out);
	bool m_bReadRgbFirst;

public:
	XPlyPointPts(XPlyNuage* parent, bool readRgbFirst = false) : XPlyPoint(parent){ m_bReadRgbFirst = readRgbFirst; }

	virtual uint16 sizeRecord(){return 19;}
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
	virtual XPlyPoint* Clone();

	virtual float Intensity(){return m_record.I;}
	void Intensity(float val){m_record.I = val;}

};

#endif //_XPLYPOINTPTS_H