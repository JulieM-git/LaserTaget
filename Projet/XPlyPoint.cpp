#include "XPlyPoint.h"
#include "XPlyNuage.h"

XPlyPoint::XPlyPoint(XPlyNuage* parent)
{
	pparent = parent;
}
//-----------------------------------------------------------------------------
//lorsque le ply géoref stocke les coordonnées de la station on peut reconstituer un x y z +- original
//pour calculer une distance
XPt3D XPlyPoint::PositionOriginale()
{
	if(pparent==NULL) 
		return Position();
	return Position() - pparent->Station();
}
