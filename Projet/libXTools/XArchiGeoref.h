#ifndef _X_ARCHI_GEOREF_H_
#define _X_ARCHI_GEOREF_H_

#include "XPt3D.h"
#include "XMat3D.h"

//Données relatives au géoréférencement
class XArchiGeoref
{
protected ://données membres
	XPt3D			m_Translation;	// Rattachement dans le référentiel véhicule
	double       m_dEchelle; //inutilisé pour l'instant !

	//données calculées
	//Les données sont exprimées dans le sens : repère capteur --> repère véhicule
	XMat3D m_Rotation;
public :
	XArchiGeoref(XPt3D T = XPt3D(), XMat3D M= XMat3D::Identite())
	{
		m_Translation = T;
		m_Rotation = M;
		m_dEchelle = 1.;
	}
	void Translation(XPt3D p){m_Translation = p;}
	XPt3D Translation(){return m_Translation;}

	XMat3D Rotation(){return m_Rotation;}
	void Rotation(XMat3D M){m_Rotation = M;}

	XPt3D Applique_transfo(XPt3D Pt);//repère capteur --> repère véhicule
	XPt3D Applique_inverse_transfo(XPt3D Pt);//repère véhicule --> repère capteur

	XMat3D Applique_transfo(XMat3D M);
	XMat3D Applique_inverse_transfo(XMat3D M);

	XArchiGeoref Applique_transfo(XArchiGeoref G);
	XArchiGeoref Applique_inverse_transfo(XArchiGeoref G);

	std::string InfoTexte();
	bool XmlWrite(std::ostream* out);

};

#endif
