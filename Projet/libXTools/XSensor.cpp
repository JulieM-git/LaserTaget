#include "XSensor.h"
#include "XPath.h"
#include "XStringTools.h"

#include "XError.h"

#include <fstream>
#include <sstream>



//-----------------------------------------------------------------------------
// Exprime le defaut par rapport a une nouvelle origine image
//-----------------------------------------------------------------------------
bool XDefect::SetNewOrigin(uint32 x, uint32 y)
{
	if ((x > m_Rect.X)||(y > m_Rect.Y))
		return false;
	m_Rect.X -= x;
	m_Rect.Y -= y;
	return true;
}

//-----------------------------------------------------------------------------
// Export sous forme de tables pour une BD relationnelle
//-----------------------------------------------------------------------------
bool XDefect::Export_TA_Database(std::ostream* out, const char* header, char sep)
{
	*out << header << sep;
	switch(m_Type) {
		case eTemperature: *out << "Pixel chaud" << sep;
			break;
		case eGap: *out << "Gap" << sep;
			break;
		case eCluster: *out << "Cluster" << sep;
			break;
		default : *out << "Inconnu" << sep;
	}
	*out << m_Rect.X << sep <<  m_Rect.Y << m_Rect.W << sep <<  m_Rect.H << sep << m_nValue << std::endl;

	return out->good();
}
//-----------------------------------------------------------------------------
bool XDefect::Export_TA_MetaDonnees(std::ostream* out)
{
	*out << "<defaut>" << std::endl;
	*out << "<type> ";
	switch(m_Type) {
	case eTemperature:
		*out << "HotPix" << " </type>";
		break;
	case eGap:
		*out << "Gap" << " </type>";
		break;
	case eCluster:
		*out << "Cluster" << " </type>";
		break;
	default:
		*out << "???" << " </type>";
	}
	*out << "<localisation> " << std::endl;
	m_Rect.XmlWrite(out);	
	*out << " </localisation>" << std::endl;

	*out << "<valeur> " << m_nValue << " </valeur>" << std::endl;
	*out << "</defaut>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XDefect::XmlWrite(std::ostream* out)
{
	*out << "<defect>" << std::endl;
	*out << "<type> ";
	switch(m_Type) {
	case eTemperature:
		*out << "HotPix" << " </type>";
		break;
	case eGap:
		*out << "Gap" << " </type>";
		break;
	case eCluster:
		*out << "Cluster" << " </type>";
		break;
	default:
		*out << "???" << " </type>";
	}

	m_Rect.XmlWrite(out);
	*out << "<value> " << m_nValue << " </value>" << std::endl;
	*out << "</defect>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Export sous forme de tables pour une BD relationnelle
//-----------------------------------------------------------------------------
bool XColorInfo::Export_TA_Database(std::ostream* out, const char* header, char sep)
{
	*out << header << sep << m_nId << sep;
	return out->good();
}
//-----------------------------------------------------------------------------
bool XColorInfo::Export_TA_MetaDonnees(std::ostream* out)
{
	*out << "<info_colorimetrique>" << std::endl;
	*out << "<id_canal> "<< m_nId << "</id_canal>" << std::endl;
	*out << "<nom_canal> "<< m_strName << "</nom_canal>" << std::endl;
	*out << "<facteur> "<< m_dFactor << "</facteur>" << std::endl;
	*out << "<rouge> "<< m_dR << "</rouge>" << std::endl;
	*out << "<vert> "<< m_dG << "</vert>" << std::endl;
	*out << "<bleu> "<< m_dB << "</bleu>" << std::endl;
	*out << "</info_colorimetrique>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XColorInfo::XmlWrite(std::ostream* out)
{
	*out << "<color_info>" << std::endl;
	*out << "<channel_id> "<< m_nId << "</channel_id>" << std::endl;
	*out << "<channel_name> "<< m_strName << "</channel_name>" << std::endl;
	*out << "<factor> "<< m_dFactor << "</factor>" << std::endl;
	*out << "<red> "<< m_dR << "</red>" << std::endl;
	*out << "<green> "<< m_dG << "</green>" << std::endl;
	*out << "<blue> "<< m_dB << "</blue>" << std::endl;
	*out << "</color_info>" << std::endl;

	return out->good();
}

//-----------------------------------------------------------------------------
// Export sous forme de tables pour une BD relationnelle
//-----------------------------------------------------------------------------
bool XFiducial::Export_TA_Database(std::ostream* out, const char* header, char sep)
{
        out->precision(2);						// Sauvegarde des parametres du flux
        out->setf(std::ios::fixed);

	*out << header << sep << m_nNum << sep;
	switch(m_Type) {
		case eZeiss: *out << "Zeiss" << sep;
		break;
		case eLeica: *out << "Zeiss" << sep;
		break;
		default : *out << "Inconnu" << sep;
	}
	*out << X << sep << Y << std::endl;

	return out->good();
}
//-----------------------------------------------------------------------------
bool XFiducial::Export_TA_MetaDonnees(std::ostream* out)
{
	*out << "<repere>" << std::endl;
	*out << "<numero> "<< m_nNum << "</numero>" << std::endl;
	*out << "<type> "<< (int)m_Type << "</type>" << std::endl;
	*out << "<point>" << std::endl;
	XPt2D::XmlWrite(out);
	*out << "</point> " << std::endl;
	*out << "</repere>" << std::endl;
	return out->good();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XFiducial::XmlWrite(std::ostream* out)
{
	*out << "<fiducial>" << std::endl;
	*out << "<number> "<< m_nNum << "</number>" << std::endl;
	*out << "<type> "<< (int)m_Type << "</type>" << std::endl;
	*out << "<x> "<< X << "</x>" << std::endl;
	*out << "<y> "<< Y << "</y>" << std::endl;
	*out << "</fiducial>" << std::endl;

	return out->good();
}
//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
XSensor::XSensor(std::string origine)
{
	m_bArgentique = false;
	m_strOrigine = origine;
	m_Orientation = N2S;
	m_dPixelSize = m_dR3 = m_dR5 = m_dR7 = 0.;
	m_DistoValue = NULL;
	m_bDistoReady = false;
	m_dScanW = 0;
	m_distoData = NULL;
	m_rawFormat = FORMAT_UNDEF;
	m_error = NULL;
}

//-----------------------------------------------------------------------------
// Cosntructeur de copie
//-----------------------------------------------------------------------------
XSensor::XSensor(const XSensor& S)
{
	m_strName = S.m_strName;
	m_strObjectif = S.m_strObjectif;
	m_strOrigine = S.m_strOrigine;
	m_strFile = S.m_strFile;
	m_strFileOrigine = S.m_strFileOrigine;
	m_strDate = S.m_strDate;
	m_strSerial = S.m_strSerial;
	m_bArgentique = S.m_bArgentique;
	m_Dark = S.m_Dark;
	m_Rect = S.m_Rect;
	m_dScanW = S.m_dScanW;
	m_rawFormat = S.m_rawFormat;

	DeleteDefects();
	uint32 i;
	for (i = 0; i < S.NbDefect(); i++){
		XDefect* newDefect = new XDefect;
		*newDefect = *(S.m_Defect[i]);
		m_Defect.push_back(newDefect);
	}
	DeleteColors();
	for (i = 0; i < S.NbColor(); i++){
		XColorInfo* newColor = new XColorInfo;
		*newColor = *(S.m_Color[i]);
		m_Color.push_back(newColor);
	}
	DeleteFiducials();
	for (i = 0; i < S.NbFiducial(); i++){
		XFiducial* newFid = new XFiducial;
		*newFid = *(S.m_Fiducial[i]);
		m_Fiducial.push_back(newFid);
	}
	m_Focal = S.m_Focal;
	m_PPS = S.m_PPS;
	m_dR3 = S.m_dR3;
	m_dR5 = S.m_dR5;
	m_dR7 = S.m_dR7;
	m_dPixelSize = S.m_dPixelSize;
	m_Orientation = S.m_Orientation;

	m_DistoValue = NULL;


	LoadDistorsion();
	m_distoData = NULL; //l'image de la distorsion en mémoire n'est pas recopiée
	m_error = S.m_error;


}
//-----------------------------------------------------------------------------
// Operateur de copie
//-----------------------------------------------------------------------------
XSensor& XSensor::operator=(const XSensor& S)
{
	if (this != &S) {
		m_strName = S.m_strName;
		m_strObjectif = S.m_strObjectif;
		m_strOrigine = S.m_strOrigine;
		m_strFile = S.m_strFile;
		m_strFileOrigine = S.m_strFileOrigine;
		m_strDate = S.m_strDate;
		m_strSerial = S.m_strSerial;
		m_bArgentique = S.m_bArgentique;
		m_Dark = S.m_Dark;
		m_Rect = S.m_Rect;
		m_dScanW = S.m_dScanW;
		m_rawFormat = S.m_rawFormat;

		DeleteDefects();
		uint32 i;
		for (i = 0; i < S.NbDefect(); i++){
			XDefect* newDefect = new XDefect;
			*newDefect = *(S.m_Defect[i]);
			m_Defect.push_back(newDefect);
		}
		DeleteColors();
		for (i = 0; i < S.NbColor(); i++){
			XColorInfo* newColor = new XColorInfo;
			*newColor = *(S.m_Color[i]);
			m_Color.push_back(newColor);
		}
		DeleteFiducials();
		for (i = 0; i < S.NbFiducial(); i++){
			XFiducial* newFid = new XFiducial;
			*newFid = *(S.m_Fiducial[i]);
			m_Fiducial.push_back(newFid);
		}
		m_Focal = S.m_Focal;
		m_PPS = S.m_PPS;
		m_dR3 = S.m_dR3;
		m_dR5 = S.m_dR5;
		m_dR7 = S.m_dR7;
		m_dPixelSize = S.m_dPixelSize;
		m_Orientation = S.m_Orientation;


		LoadDistorsion();
		m_distoData = NULL; //l'image de la distorsion en mémoire n'est pas recopiée
		m_error = S.m_error;

	}

	return *this;
}

//-----------------------------------------------------------------------------
// Destructeur
//-----------------------------------------------------------------------------
XSensor::~XSensor()
{
	DeleteDefects();
	DeleteColors();
	DeleteFiducials();

	UnloadDistorsion();

	if(m_distoData != NULL)//disto image
		delete m_distoData;

}
//-----------------------------------------------------------------------------
double XSensor::Ouverture_W(bool applyDistorsion)
{ 
	if(applyDistorsion)
	{
		if((!m_bDistoReady)&&(!LoadDistorsion(m_error)))
			return 0.;
		XPt2D gauche = XPt2D(0,m_PPS.Y);
		XPt2D gauchecor = SubDistorsion(gauche);
		double cgauche = m_PPS.X - gauchecor.X;
		double ogauche = 180* atan(cgauche/m_Focal.Z)/M_PI;	

		XPt2D droit = XPt2D(m_Rect.W,m_PPS.Y);
		XPt2D droitcor = SubDistorsion(droit);
		double cdroit =  droitcor.X- m_PPS.X;
		double odroit = 180* atan(cdroit/m_Focal.Z)/M_PI;	
		return ogauche + odroit;	
	}
	return 2*180* atan((m_Rect.W/2.0)/m_Focal.Z)/M_PI;
}
//-----------------------------------------------------------------------------
double XSensor::Ouverture_H(bool applyDistorsion)
{ 
	if(applyDistorsion)
	{
		if((!m_bDistoReady)&&(!LoadDistorsion(m_error)))
			return 0.;
		XPt2D haut = XPt2D(m_PPS.X,0);
		XPt2D hautcor = SubDistorsion(haut);
		double chaut = m_PPS.Y - hautcor.Y;
		double ohaut = 180* atan(chaut/m_Focal.Z)/M_PI;	

		XPt2D bas = XPt2D(m_PPS.X,m_Rect.H);
		XPt2D bascor = SubDistorsion(bas);
		double cbas =  bascor.Y- m_PPS.Y;
		double obas = 180* atan(cbas/m_Focal.Z)/M_PI;	
		return ohaut+obas;	
	}
	return 2*180* atan((m_Rect.H/2.0)/m_Focal.Z)/M_PI;
}
//-----------------------------------------------------------------------------
double XSensor::Ouverture_Horizontale(bool applyDistorsion)
{ 
	if((m_Orientation == XSensor::N2S) || (m_Orientation == XSensor::S2N))
		return Ouverture_H(applyDistorsion);
	return Ouverture_W(applyDistorsion);
}
//-----------------------------------------------------------------------------
double XSensor::Ouverture_Verticale(bool applyDistorsion)
{ 
	if((m_Orientation == XSensor::N2S) || (m_Orientation == XSensor::S2N))
		return Ouverture_W(applyDistorsion);
	return Ouverture_H(applyDistorsion);
}

//-----------------------------------------------------------------------------
// Fixe l'origine (si l'origine est "Argentique", le capteur devient argentique)
//-----------------------------------------------------------------------------
void XSensor::Origine(std::string Origine)
{
	m_strOrigine = Origine;
	if ((m_strOrigine.compare("Argentique")==0) || (m_strOrigine.compare("Argentique_tournee")==0))
		m_bArgentique = true;
}

//-----------------------------------------------------------------------------
// Destruction de la liste des defauts
//-----------------------------------------------------------------------------
void XSensor::DeleteDefects()
{
	for (uint32 i = 0; i < NbDefect(); i++)
		delete m_Defect[i];
	m_Defect.clear();
}

//-----------------------------------------------------------------------------
// Destruction de la liste des informations sur les couleurs
//-----------------------------------------------------------------------------
void XSensor::DeleteColors()
{
	for (uint32 i = 0; i < NbColor(); i++)
		delete m_Color[i];
	m_Color.clear();
}

//-----------------------------------------------------------------------------
// Destruction de la liste des reperes de fond de chambre
//-----------------------------------------------------------------------------
void XSensor::DeleteFiducials()
{
	for (uint32 i = 0; i < NbFiducial(); i++)
		delete m_Fiducial[i];
	m_Fiducial.clear();
}


//-----------------------------------------------------------------------------
// Exprime les defauts par rapport a la zone utile
//-----------------------------------------------------------------------------
void XSensor::SetUsefullOrigin()
{
	for (uint32 i = 0; i < NbDefect(); i++)
		m_Defect[i]->SetNewOrigin(m_Rect.X, m_Rect.Y);
}

//-----------------------------------------------------------------------------
// Renvoie les informations colorimetriques d'un canal
//-----------------------------------------------------------------------------
XColorInfo* XSensor::GetColor(uint32 id)
{
	for (uint32 i = 0; i < NbColor(); i++){
		if (m_Color[i]->Id() == id)
			return m_Color[i];
	}
	return NULL;
}

double XSensor::GetChannelFactor(uint32 id)
{
	for (uint32 i = 0; i < NbColor(); i++){
		if (m_Color[i]->Id() == id)
			return m_Color[i]->Factor();
	}
	return 1.0;
}

//-----------------------------------------------------------------------------
// Ajoute des défauts
//-----------------------------------------------------------------------------
void XSensor::AddDefects(std::vector<XDefect*> VectDef)
{
	std::vector<XDefect*>::iterator iter;
	for (iter = VectDef.begin(); iter != VectDef.end(); iter++) {
		XDefect* newDefect = new XDefect;
		*newDefect = *(*iter);
		m_Defect.push_back(newDefect);
	}
}

//-----------------------------------------------------------------------------
// Ajoute des informations colorimetriques
//-----------------------------------------------------------------------------
void XSensor::AddColors(std::vector<XColorInfo*> VectCol)
{
	std::vector<XColorInfo*>::iterator iter;
	for (iter = VectCol.begin(); iter != VectCol.end(); iter++) {
		XColorInfo* newColor = new XColorInfo;
		*newColor = *(*iter);
		m_Color.push_back(newColor);
	}
}

//-----------------------------------------------------------------------------
// Ajoute des reperes de fond de chambre
//-----------------------------------------------------------------------------
void XSensor::AddFiducials(std::vector<XFiducial*> VectFid)
{
	std::vector<XFiducial*>::iterator iter;
	for (iter = VectFid.begin(); iter != VectFid.end(); iter++) {
		XFiducial* newFid = new XFiducial;
		*newFid = *(*iter);
		m_Fiducial.push_back(newFid);
	}
}

//-----------------------------------------------------------------------------
// Passage repere photogrammetrique -> repere image
//-----------------------------------------------------------------------------
XPt2D XSensor::Cli2Ima(XPt2D M)
{

	switch(m_Orientation)
	{
		case N2S :
			return AddDistorsion(XPt2D(M.Y + m_Focal.X, M.X + m_Focal.Y));
		case S2N :
			return AddDistorsion(XPt2D(m_Focal.X - M.Y, m_Focal.Y - M.X));
		case W2E :
			return AddDistorsion(XPt2D(M.X + m_Focal.X, m_Focal.Y - M.Y));
		case E2W :
			return AddDistorsion(XPt2D(m_Focal.X - M.X, m_Focal.Y + M.Y));
	}
	return XPt2D(0, 0);
}
//-----------------------------------------------------------------------------
// Passage repere photogrammetrique pk1 -> repere image
//-----------------------------------------------------------------------------
XPt2D XSensor::Pk12Ima(XPt2D M)
{
	switch(m_Orientation) 
	{
		case N2S :
			return AddDistorsion(XPt2D(M.Y + m_Focal.X, M.X + m_Focal.Y));
		case S2N :
			return AddDistorsion(XPt2D(m_Focal.X - M.Y, m_Focal.Y - M.X));
		case W2E :
			return AddDistorsion(XPt2D(M.X + m_Focal.X, m_Focal.Y - M.Y));
		case E2W :
			return AddDistorsion(XPt2D(m_Focal.X - M.X, m_Focal.Y + M.Y));
	}
	return XPt2D(0, 0);
}
//-----------------------------------------------------------------------------
// Passage repere image -> repere photogrammetrique
//-----------------------------------------------------------------------------
XPt3D XSensor::Ima2Cli(XPt2D P)
{
	XPt2D M = P;

	if (m_bDistoReady)
		M = SubDistorsion(P);

	switch(m_Orientation) {
		case N2S :
			return XPt3D(M.Y - m_Focal.Y, M.X - m_Focal.X, -m_Focal.Z);
		case S2N :
			return XPt3D(m_Focal.Y - M.Y, m_Focal.X - M.X, -m_Focal.Z);
		case W2E :
			return XPt3D(M.X - m_Focal.X, m_Focal.Y - M.Y, -m_Focal.Z);
		case E2W :
			return XPt3D(m_Focal.X - M.X, M.Y - m_Focal.Y, -m_Focal.Z);
	}
	return XPt2D(0, 0);
}
//-----------------------------------------------------------------------------
// Passage repere image -> repere photogrammetrique pk1
//-----------------------------------------------------------------------------
XPt3D XSensor::Ima2Pk1(XPt2D P)
{
	XPt2D M = CorrigeDistorsion(P);
	switch(m_Orientation) {
		case N2S :
			return XPt3D(M.Y - m_Focal.Y, M.X - m_Focal.X, -m_Focal.Z);
		case S2N :
			return XPt3D(m_Focal.Y - M.Y, m_Focal.X - M.X, -m_Focal.Z);
		case W2E :
			return XPt3D(M.X - m_Focal.X, m_Focal.Y - M.Y, -m_Focal.Z);
		case E2W :
			return XPt3D(m_Focal.X - M.X, M.Y - m_Focal.Y, -m_Focal.Z);
	}
	return XPt2D(0, 0);
}
//-----------------------------------------------------------------------------
// Renvoie le nadir superieur de l'image en fonction de l'orientation du capteur
//-----------------------------------------------------------------------------
uint32 XSensor::NadirSup()
{
	switch(m_Orientation) {
		case N2S :
			return 3;
		case S2N :
			return 7;
		case W2E :
			return 1;
		case E2W :
			return 5;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Renvoie le nadir inferieur de l'image en fonction de l'orientation du capteur
//-----------------------------------------------------------------------------
uint32 XSensor::NadirInf()
{
	switch(m_Orientation) {
		case N2S :
			return 7;
		case S2N :
			return 3;
		case W2E :
			return 5;
		case E2W :
			return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Renvoie la largeur du capteur en fonction de l'orientation du capteur
//-----------------------------------------------------------------------------
uint32 XSensor::FlightW()
{
	switch(m_Orientation) {
		case N2S :
		case S2N :
			return Height();
		case W2E :
		case E2W :
			return Width();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Renvoie la hauteur du capteur en fonction de l'orientation du capteur
//-----------------------------------------------------------------------------
uint32 XSensor::FlightH()
{
	switch(m_Orientation) {
		case N2S :
		case S2N :
			return Width();
		case W2E :
		case E2W :
			return Height();
	}
	return 0;
}
//-----------------------------------------------------------------------------
// Renvoie la position du point focal dans le repère photogrammétrique
//-----------------------------------------------------------------------------
double XSensor::FlightFocal()
{
	switch(m_Orientation) {
		case N2S :
			return  m_Focal.X;
		case S2N :
			return  Width()-m_Focal.X ;
		case W2E :
			return  m_Focal.Y ;
		case E2W :
			return Height()- m_Focal.Y;
	}
	return Width()/2;
}
/*a virer
//-----------------------------------------------------------------------------
// Tourne une matrice rotation en fonction de l'orientation du capteur
//-----------------------------------------------------------------------------
XMat3D XSensor::MatriceORI(XMat3D Apx)
{
	switch(m_Orientation) {
		case N2S :
			return Apx;
		case S2N :
			return Apx*XMat3D::MPI();
		case W2E :
			return Apx;
		case E2W :
			return Apx;
	}
	return 0;
}*/
//-----------------------------------------------------------------------------
// Libération de la mémoire occupée par les parametres internes de calcul de distortion
//-----------------------------------------------------------------------------
void XSensor::UnloadDistorsion()
{
	m_bDistoReady = false;

	if (m_DistoValue != NULL)
		delete[] m_DistoValue;
	m_DistoValue = NULL;

}
//-----------------------------------------------------------------------------
// Preparation des parametres internes de calcul de distortion
//-----------------------------------------------------------------------------
bool XSensor::LoadDistorsion(XError* error)
{
	m_bDistoReady = false;
	if (m_DistoValue != NULL)
		delete[] m_DistoValue;
	m_DistoValue = NULL;

	if ((m_dR3 == 0.)&&(m_dR5 == 0.)&&(m_dR7 == 0.))
		return true;

	double step = 10.;	// Pas de la grille
	m_PPScli = Ima2Cli(m_PPS);
	double xmax= XMax(m_PPS.X,m_Rect.W-m_PPS.X);
	double ymax= XMax(m_PPS.Y,m_Rect.H-m_PPS.Y);
	double diag = sqrt((double)xmax *xmax + ymax * ymax);
	m_nNbDistoValue = (uint16)( diag / step) +2; 

	// Creation du tableau de valeur
	m_DistoValue = new XPt2D[m_nNbDistoValue];
	if (m_DistoValue == NULL)
		return false;

	for(uint16 i = 0; i < m_nNbDistoValue; i++) {
		double r = (double)i * step;
		m_DistoValue[i].X = r;
		m_DistoValue[i].Y = r + (m_dR3 + (m_dR5 + m_dR7 * r * r) * r * r) * r * r * r;
	}

	m_bDistoReady = true;
	return m_bDistoReady;
}

//-----------------------------------------------------------------------------
// Passage image avec distortion -> image sans distortion
//-----------------------------------------------------------------------------
XPt2D XSensor::CorrigeDistorsion(XPt2D A)
{
	return SubDistorsion(A);
}
//-----------------------------------------------------------------------------
XPt2D XSensor::SubDistorsion(XPt2D A)
{
	if (!m_bDistoReady)
		return A;

	if(A == m_PPS)
		return A;

	XPt2D M = A - m_PPS;
	double r = sqrt(M.X * M.X + M.Y * M.Y);
	if (r > m_DistoValue[m_nNbDistoValue - 1].X)
		return A;

	double cos_theta = M.X / r;
	double sin_theta = M.Y / r;

    uint16 val =0;
	for(val = 0; val < m_nNbDistoValue - 1; val++)
		if ((r >= m_DistoValue[val].X)&&(r <= m_DistoValue[val+1].X))  
			break;
		

	double a = (r - m_DistoValue[val].Y) / (m_DistoValue[val+1].Y - m_DistoValue[val].Y);
	double r_cor = m_DistoValue[val].X + a * (m_DistoValue[val+1].X - m_DistoValue[val].X);

	M.X = r_cor * cos_theta;
	M.Y = r_cor * sin_theta;
	M += m_PPS;

	return M;
}


//-----------------------------------------------------------------------------
// Passage image sans distortion -> image avec distortion
//-----------------------------------------------------------------------------
XPt2D XSensor::AddDistorsion(XPt2D A)
{
	if (!m_bDistoReady)
		return A;

	if(A == m_PPS)
		return A;

	XPt2D M = A - m_PPS;
	double r2 = (M.X * M.X + M.Y * M.Y);
	double dr = 1. + (m_dR3 + (m_dR5 + m_dR7 * r2) * r2) * r2;

	M.X = (M.X * dr);
	M.Y = (M.Y * dr);
	M += m_PPS;

	return M;
}
//-----------------------------------------------------------------------------
bool XSensor::TerrainToPhoto(double xIn, double yIn, double &xOut, double &yOut)
{
		xIn *= m_Focal.Z;
		yIn *= m_Focal.Z;
		xIn += m_Focal.X;
		yIn += m_Focal.Y;
		XPt2D M = AddDistorsion( XPt2D(xIn,yIn));
		xOut = M.X;
		yOut = M.Y;
		return true;
	
}

//-----------------------------------------------------------------------------
bool XSensor::PhotoToTerrain(double xIn, double yIn, double &xOut, double &yOut)
{
	XPt2D M = SubDistorsion(XPt2D(xIn,yIn));
	xOut = M.X;
	yOut = M.Y;
	xOut -= m_Focal.X;
	yOut -= m_Focal.Y;
	xOut /= m_Focal.Z;
	yOut /= m_Focal.Z;
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int XSensor::CodeDistorsionOri()
{
	if((m_dR3 != 0)||(m_dR5 != 0)||(m_dR7 != 0))//2 distorsion radiale
		return 2;
	return 0;//pas de distorison
}
//-----------------------------------------------------------------------------
//Ecriture de la calibration au format cam pour comp3D
//-----------------------------------------------------------------------------
bool XSensor::WriteCamComp3D(std::ostream* out)
{
//1633.210    1292.845    2324.018   1607.258        1272.910
//-1.114545E-0008  7.545268E-0018  3.255540E-0023
//0
//3264 2448

	//à vérifier ****  !!
	double YppaComp = m_Rect.H - m_Focal.Y;
	double YppsComp = m_Rect.H - m_PPS.Y;

	char sep = '\t';
	out->precision(3);
	*out << m_Focal.X <<sep<< YppaComp <<sep<< m_Focal.Z <<sep<< m_PPS.X <<sep<< YppsComp << std::endl;

	out->precision(8);
	*out << std::scientific ;
	*out << m_dR3 <<sep<< m_dR5 <<sep<< m_dR7 << std::endl;
	*out << "0" << std::endl;
		
	*out << std::fixed ;
	*out << m_Rect.W <<sep<< m_Rect.H << std::endl;

	return out->good();
}
//-----------------------------------------------------------------------------
//Ecriture de la calibration au format dat pour TopAéro
//-----------------------------------------------------------------------------
bool XSensor::WriteCamTopAero(std::ostream* out)
{
/*
Type            : NUM
Dim             : 4096  4096
Symetrie        : 3124.030 -1682.736
AutoCollimation : 1981.385 -2021.378
Focale          : 11253.530
RadiusCount     : 0
CoefDistortion  : 3.994807e-11  1.111509e-18  -3.904922e-26
END
*/
	*out << "Type            : NUM" << std::endl;
	*out << "Dim             : " << m_Rect.W  <<"  "<< m_Rect.H  << std::endl;
	*out << "Symetrie        : " << m_PPS.X   <<" " << -m_PPS.Y   << std::endl;
	*out << "AutoCollimation : " << m_Focal.X <<" " << -m_Focal.Y << std::endl;
	*out << "Focale          : " << m_Focal.Z << std::endl;
	*out << "RadiusCount     : 0" << std::endl;
	*out << "CoefDistortion  : " << m_dR3 <<"  "<< m_dR5 <<"  "<< m_dR7 << std::endl;
	*out << "END" << std::endl;

	return out->good();
}
//-----------------------------------------------------------------------------
XMat3D XSensor::MatOrientationCapteur()
{
	XMat3D Identite = XMat3D(XPt3D(1,0,0), 
							XPt3D(0,1,0),
							XPt3D(0,0,1));
	switch(m_Orientation) {
		case N2S :
			return XMat3D(XPt3D(0,-1,0),
						  XPt3D(1,0,0),
						  XPt3D(0,0,1));
		case S2N :
			return XMat3D(XPt3D(0,1,0),
						  XPt3D(-1,0,0),
						  XPt3D(0,0,1));
		case E2W :
			return XMat3D(XPt3D(-1,0,0),
						  XPt3D(0,-1,0),
						  XPt3D(0,0,1));
		case W2E :
			return Identite;
	}
	return Identite;
}

//-----------------------------------------------------------------------------
std::string XSensor::DescriptionTexte()
{
	std::ostringstream oss;
	oss << "Id: "<< Serie() << "\torigine: " << Origine()<< "\tnom: " << Name();
	oss << "\r\n\tTaille :\tL= " << Width() << "\tH= " << Height()<< "\tPix= " << PixelSize()<< "\tObjectif= " << Objectif();
	oss << "\r\n\tFocale : " << Focal().Z << "\tPPA : x= " << Focal().X<< "\ty= " << Focal().Y << "\tOrientation : " << (int)Orientation();
	oss  <<"\r\n\tOuverture horizontale= " << Ouverture_Horizontale(false)<< "°\tVerticale= " << Ouverture_Verticale(false)<< "°";
	oss  <<"\r\n\tOuverture horizontale= " << Ouverture_Horizontale(true)<< "°\tVerticale= " << Ouverture_Verticale(true)<< "° (distorsion appliquée)";
	oss << "\r\n\tFormat natif : " << RawFormatToString(m_rawFormat) << "\t FlatField : " ;
	

	
		XPt3D polynome = DistoPolynome();
		if ((Width() == 4096)&&(Height() == 4096)&&(polynome == XPt3D())){
			oss <<  "\r\nPAS DE CALIBRATION";
		}
		else{
			oss <<  "\tDistorsion par polynôme";
			oss << "\r\n\tPPS\tX = " << PPS().X << "\tY = " << PPS().Y;
			oss.precision(8);
			oss << "\r\n\tr3 = " << polynome.X;
			oss << "\tr5 = " << polynome.Y;
			oss << "\tr7 = " << polynome.Z;
			oss << "\r\n\tDate d'étalonnage : " << Date();
			oss << "\r\n\tFichier original : " << FilenameOrigine();
		}
	
	return oss.str();
}

//-----------------------------------------------------------------------------
// Ecriture dans un fichier XML
//-----------------------------------------------------------------------------
bool XSensor::XmlWrite(std::ostream* out)
{
	int prec = out->precision(2);

	*out << "<sensor>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	*out << "<objectif> " << m_strObjectif << " </objectif>" << std::endl;
	*out << "<origine> " << m_strOrigine << " </origine>" << std::endl;
	if(m_bArgentique)
		*out << "<argentique> " << (int)m_bArgentique << " </argentique>" << std::endl;
	if (m_strDate.size() > 0)
		*out << "<calibration-date> " << m_strDate << " </calibration-date>" << std::endl;
	if (m_strSerial.size() > 0)
		*out << "<serial-number> " << m_strSerial << " </serial-number>" << std::endl;

	*out << "<raw-format>" << RawFormatToString(m_rawFormat) << "</raw-format>" << std::endl;
	*out << "<usefull-frame> " << std::endl;
	m_Rect.XmlWrite(out);
	*out << " </usefull-frame>" << std::endl;
	//L'ecriture du dark-frame doit être systématique car obligatoire en lecture
		*out << "<dark-frame> " << std::endl;
		m_Dark.XmlWrite(out);
		*out << " </dark-frame>" << std::endl;

    uint32 i;
	for (i = 0; i < NbDefect(); i++)
		m_Defect[i]->XmlWrite(out);
	for (i = 0; i < NbColor(); i++)
		m_Color[i]->XmlWrite(out);
	out->precision(3);
	for (i = 0; i < NbFiducial(); i++)
		m_Fiducial[i]->XmlWrite(out);

	*out << "<focal>" << std::endl;
	m_Focal.XmlWrite(out);
	*out << "</focal>" << std::endl;

	if ((m_dR3 != 0.0)&&(m_dR5 != 0.0)&&(m_dR7 != 0.0)) {
		*out << "<distortion>" << std::endl;
		m_PPS.XmlWrite(out);
		out->precision(8);
		*out << std::scientific ;
		*out << "<r3> " << m_dR3 << "</r3>" << std::endl;
		*out << "<r5> " << m_dR5 << "</r5>" << std::endl;
		*out << "<r7> " << m_dR7 << "</r7>" << std::endl;
		*out << "</distortion>" << std::endl;
	}


	out->precision(8);
	*out << std::fixed ;
	*out << "<pixel_size> " << m_dPixelSize << "</pixel_size>" << std::endl;

	*out << "<orientation> " << m_Orientation << "</orientation>" << std::endl;

	out->precision(3);
	if (m_dScanW != 0.0)
		*out << "<scan_width> " << m_dScanW << " </scan_width>" << std::endl;

	if(m_strFileOrigine.size() != 0)
		*out << "<file_origine> " << m_strFileOrigine << " </file_origine>" << std::endl;
	*out << "</sensor>" << std::endl;

	out->precision(prec);		// Restauration des parametres du flux
	return out->good();
}
//-----------------------------------------------------------------------------
// Ecriture dans un fichier Ori Xml nouvelle génération
//-----------------------------------------------------------------------------
bool XSensor::XmlImageWrite(std::ostream* out)
{
	int prec = out->precision(2);						// Sauvegarde des parametres du flux
	std::ios::fmtflags flags = out->setf(std::ios::fixed);

	*out << "<sensor>" << std::endl;
	*out << "<name> " << m_strName << " </name>" << std::endl;
	if(!m_strObjectif.empty())
		*out << "<objectif> " << m_strObjectif << " </objectif>" << std::endl;
	if (m_strDate.size() > 0)
		*out << "<calibration_date> " << m_strDate << " </calibration_date>" << std::endl;
	if (m_strSerial.size() > 0)
		*out << "<serial_number> " << m_strSerial << " </serial_number>" << std::endl;

	*out << "<image_size> " << std::endl;
	*out << "<width> " << m_Rect.W << " </width>" << std::endl;
	*out << "<height> " << m_Rect.H << " </height>" << std::endl;
	*out << " </image_size>" << std::endl;

	out->precision(3);
	*out << "<ppa>" << std::endl;
		*out << "<c> " << m_Focal.X << " </c>" << std::endl;
		*out << "<l> " << m_Focal.Y << " </l>" << std::endl;
		*out << "<focale> " << m_Focal.Z << " </focale>" << std::endl;
	*out << "</ppa>" << std::endl;

	if ((m_dR3 != 0.0)&&(m_dR5 != 0.0)&&(m_dR7 != 0.0)) {
		*out << "<distortion>" << std::endl;
		*out << "<pps>" << std::endl;
			*out << "<c> " << m_PPS.X << " </c>" << std::endl;
			*out << "<l> " << m_PPS.Y << " </l>" << std::endl;
		*out << "</pps>" << std::endl;

		out->precision(8);
		*out << std::scientific ;
		*out << "<r3> " << m_dR3 << "</r3>" << std::endl;
		*out << "<r5> " << m_dR5 << "</r5>" << std::endl;
		*out << "<r7> " << m_dR7 << "</r7>" << std::endl;
		*out << "</distortion>" << std::endl;
	}


	out->precision(8);
	*out << std::fixed ;
	*out << "<pixel_size> " << m_dPixelSize << "</pixel_size>" << std::endl;

	//pour les publications externes : l'orientation est inclue dans la matrice rotation
	//*out << "<orientation> " << m_Orientation << "</orientation>" << std::endl;
	*out << "</sensor>" << std::endl;	

	out->precision(prec);		// Restauration des parametres du flux
	out->setf(flags);
	return out->good(); 
}

//-----------------------------------------------------------------------------
int XSensor::BayerPattern()
{	
	//RGGB = 0,
	//BGGR,
	//GBRG,
	//GRBG
	switch(m_rawFormat)
	{
	case FORMAT_RAW8_RGGB:
		return 0;
		break;
	case FORMAT_RAW8_GBRG:
		return 2;
		break;
	case FORMAT_RAW8_GRBG:
		return 3;
		break;
	case FORMAT_RAW8_BGGR:
		return 1;
		break;
	case FORMAT_RAW12_IGN:
		return 3;
		break;
	case FORMAT_RAW16_RGGB:
		return 0;
		break;
	case FORMAT_RAW16_GBRG:
		return 2;
		break;
	case FORMAT_RAW16_GRBG:
		return 3;
		break;
	case FORMAT_RAW16_BGGR:
		return 1;
		break;
	default:
		return -1;;
		break;
	}	
	return -1;
}
//-----------------------------------------------------------------------------
std::string XSensor::RawFormatToString(eRawFormat format)
{
	switch(format)
	{
	case FORMAT_UNDEF:
		return std::string("FORMAT_UNDEF");
		break;
	case FORMAT_RAW12_IGN:
		return std::string("FORMAT_RAW12_IGN");
		break;
	case FORMAT_MONO8:
		return std::string("FORMAT_MONO8");
		break;
	case FORMAT_RGB8:
		return std::string("FORMAT_RGB8");
		break;
	case FORMAT_RAW8_RGGB:
		return std::string("FORMAT_RAW8_RGGB");
		break;
	case FORMAT_RAW8_GBRG:
		return std::string("FORMAT_RAW8_GBRG");
		break;
	case FORMAT_RAW8_GRBG:
		return std::string("FORMAT_RAW8_GRBG");
		break;
	case FORMAT_RAW8_BGGR:
		return std::string("FORMAT_RAW8_BGGR");
		break;
	case FORMAT_MONO16:
		return std::string("FORMAT_MONO8");
		break;
	case FORMAT_RGB16:
		return std::string("FORMAT_RGB16");
		break;
	case FORMAT_RAW16_RGGB:
		return std::string("FORMAT_RAW16_RGGB");
		break;
	case FORMAT_RAW16_GBRG:
		return std::string("FORMAT_RAW16_GBRG");
		break;
	case FORMAT_RAW16_GRBG:
		return std::string("FORMAT_RAW16_GRBG");
		break;
	case FORMAT_RAW16_BGGR:
		return std::string("FORMAT_RAW16_BGGR");
		break;
	case FORMAT_NEF:
		return std::string("FORMAT_NEF");
		break;
	case FORMAT_JPEG:
		return std::string("FORMAT_JPEG");
		break;
	default:
		return std::string("FORMAT_UNDEF");
		break;
	}	
	return std::string("FORMAT_UNDEF");
}
//-----------------------------------------------------------------------------
XSensor::eRawFormat XSensor::StringToRawFormat(std::string strFormat)
{
	if(strFormat.compare("FORMAT_UNDEF") == 0)
		return FORMAT_UNDEF;
	if(strFormat.compare("FORMAT_RAW12_IGN") == 0)
		return FORMAT_RAW12_IGN;
	if(strFormat.compare("FORMAT_MONO8") == 0)
		return FORMAT_MONO8;
	if(strFormat.compare("FORMAT_RGB8") == 0)
		return FORMAT_RGB8;
	if(strFormat.compare("FORMAT_RAW8_RGGB") == 0)
		return FORMAT_RAW8_RGGB;
	if(strFormat.compare("FORMAT_RAW8_GBRG") == 0)
		return FORMAT_RAW8_GBRG;
	if(strFormat.compare("FORMAT_RAW8_GRBG") == 0)
		return FORMAT_RAW8_GRBG;
	if(strFormat.compare("FORMAT_RAW8_BGGR") == 0)
		return FORMAT_RAW8_BGGR;
	if(strFormat.compare("FORMAT_MONO16") == 0)
		return FORMAT_MONO16;
	if(strFormat.compare("FORMAT_RGB16") == 0)
		return FORMAT_RGB16;
	if(strFormat.compare("FORMAT_RAW16_RGGB") == 0)
		return FORMAT_RAW16_RGGB;
	if(strFormat.compare("FORMAT_RAW16_GBRG") == 0)
		return FORMAT_RAW16_GBRG;
	if(strFormat.compare("FORMAT_RAW16_GRBG") == 0)
		return FORMAT_RAW16_GRBG;
	if(strFormat.compare("FORMAT_RAW16_BGGR") == 0)
		return FORMAT_RAW16_BGGR;
	if(strFormat.compare("FORMAT_NEF") == 0)
		return FORMAT_NEF;
	if(strFormat.compare("FORMAT_JPEG") == 0)
		return FORMAT_JPEG;
		
	return FORMAT_UNDEF;
}
//-----------------------------------------------------------------------------
bool XSensor::CheckAuxilliaryData()
{
	return true;
}

//convertion du numéro de série string en uint32
//-----------------------------------------------------------------------------
uint32 XSensor::SerieNum()
{
	XStringTools st;
        std::string serie=Serie();
	return st.StringToUint32(serie);
}