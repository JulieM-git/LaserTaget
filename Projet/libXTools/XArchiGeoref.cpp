#include "XArchiGeoref.h"
#include "XQuaternion.h"

#include <sstream>

//-----------------------------------------------------------------------------
//resultat = eche* Rot*Pt+Trans  //repère capteur --> repère véhicule
XPt3D XArchiGeoref::Applique_transfo(XPt3D Pt)
{
	XPt3D res = m_dEchelle * m_Rotation*Pt + m_Translation;
	return res;
}
//-----------------------------------------------------------------------------
//resultat = transpose(Rot)*(Pt-Trans)/eche //repère véhicule --> repère capteur
XPt3D XArchiGeoref::Applique_inverse_transfo(XPt3D Pt)
{
	XPt3D res = 1/m_dEchelle *(m_Rotation.Trn()*(Pt-m_Translation));
	return res;
}
//-----------------------------------------------------------------------------
XMat3D XArchiGeoref::Applique_transfo(XMat3D M)
{
	XMat3D R = m_Rotation * M;
	return R;
}
//-----------------------------------------------------------------------------
XMat3D XArchiGeoref::Applique_inverse_transfo(XMat3D M)
{
	XMat3D R = m_Rotation * M;
	return R.Trn();
}

//-----------------------------------------------------------------------------
XArchiGeoref XArchiGeoref::Applique_transfo(XArchiGeoref G)
{
	//XMat3D R = G.m_Rotation * m_Rotation ;
	XMat3D R = m_Rotation * G.m_Rotation  ;
	XPt3D Tori = m_Rotation * G.m_Translation;
	XPt3D P =  Tori + m_Translation;

	return XArchiGeoref(P,R);
}
//-----------------------------------------------------------------------------
XArchiGeoref XArchiGeoref::Applique_inverse_transfo(XArchiGeoref G)
{
	XMat3D R = m_Rotation * G.m_Rotation;
	XPt3D P = m_Rotation.Trn()*(G.m_Translation-m_Translation);

	return XArchiGeoref(P,R.Trn());
}
//-----------------------------------------------------------------------------
std::string XArchiGeoref::InfoTexte()
{
	std::ostringstream oss;
	oss << "Rattachement du sommet : " << m_Translation << std::endl;
	oss << "Matrice rotation : "  << std::endl;
	oss << m_Rotation << std::endl;

	return oss.str();
}
//-----------------------------------------------------------------------------
bool XArchiGeoref::XmlWrite(std::ostream* out)
{
	int prec = out->precision(2);// Sauvegarde des parametres du flux
	std::ios::fmtflags flags = out->setf(std::ios::fixed);

	*out << "<georef>\n";
		Translation().XmlWrite(out);
		out->precision(12);
		*out << "<rotation>\n";
        m_Rotation.XmlWrite(out);
		*out << "</rotation>\n";
	*out << "</georef>\n";

	out->precision(prec);		// Restauration des parametres du flux
	out->unsetf(std::ios::fixed);
	out->setf(flags);
	return out->good();
}

