//XSensor 
//Francois Becirspahic Projet caméra Numérique
//JP Papelard SR /Matis

#ifndef _XSENSOR_H
#define _XSENSOR_H

#include "XBase.h"
#include "XRect.h"
#include "XPt3D.h"
#include "XPt2D.h"
#include "XMat3D.h"

class XDistoImageData;
class XRawImageFlatField;
class XTraitementDistorsionParams;
class XChromaticData;
class XError;
//-----------------------------------------------------------------------------
// Classe XDefect : pour gerer les defauts (pixels morts) d'une matrice CCD
//-----------------------------------------------------------------------------
class XDefect{
public:
	enum eType { eNull, eTemperature, eGap, eCluster};
protected:
	eType	m_Type;
	XRect	m_Rect;
	uint32	m_nValue;
public:
	XDefect() : m_Type(eNull), m_Rect(0,0,0,0), m_nValue(0) {;}
	virtual ~XDefect() {;}

	inline eType Type() const { return m_Type;}
	inline XRect Rect() const { return m_Rect;}
	inline uint32 X() const { return m_Rect.X;}
	inline uint32 Y() const { return m_Rect.Y;}
	inline uint32 W() const { return m_Rect.W;}
	inline uint32 H() const { return m_Rect.H;}
	inline uint32 Value() const { return m_nValue;}

	bool SetNewOrigin(uint32 x, uint32 y);
	void SetType(eType typ) {m_Type = typ;}
	void SetRect(XRect rect){m_Rect = rect;}
	void SetValue(uint32 val){m_nValue = val;}

	bool XmlWrite(std::ostream* out);
	bool Export_TA_Database(std::ostream* out, const char* header, char sep = '\t');
	bool Export_TA_MetaDonnees(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Classe XColorInfo : gere les informations colorimetriques des matrices CCD couleur
//-----------------------------------------------------------------------------
class XColorInfo {
protected:
	uint32		m_nId;			// Identifiant du canal colorimetrique
	std::string m_strName;		// Nom du canal
	double		m_dFactor;		// Facteur pour le Flat Field
	double		m_dR;			// Facteur d'egalisation des couleurs
	double		m_dG;			// Facteur d'egalisation des couleurs
	double		m_dB;			// Facteur d'egalisation des couleurs
public:
	XColorInfo() : m_nId(0), m_dFactor(1.0) {;}
	virtual ~XColorInfo() {;}

	inline double Factor() const { return m_dFactor;}
	inline uint32 Id() const { return m_nId;}
	std::string Name() const { return m_strName;}
	inline double R() { return m_dR;}
	inline double G() { return m_dG;}
	inline double B() { return m_dB;}

	void  Factor(double f)  {m_dFactor = f;}
	void  Id(uint32 id)  { m_nId = id;}
	void Name(std::string name)  { m_strName = name;}
	void  R(double r) { m_dR = r;}
	void  G(double g) { m_dG = g;}
	void  B(double b) { m_dB = b;}

	inline double Equalize(int r, int g, int b) { return m_dR*r + m_dG*g + m_dB*b;}

	bool XmlWrite(std::ostream* out);
	bool Export_TA_Database(std::ostream* out, const char* header, char sep = '\t');
	bool Export_TA_MetaDonnees(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Classe XFiducial : gere les reperes de fond de chambre d'une camera
//-----------------------------------------------------------------------------
class XFiducial : public XPt2D {
public:
	enum eType { eNull, eZeiss, eLeica};
protected:
	eType			m_Type;			// Type de repere
	uint32		m_nNum;			// Numero
public:
	XFiducial() : XPt2D(), m_Type(eNull), m_nNum(0){;}
	virtual ~XFiducial() {;}

	inline uint32 Number() const { return m_nNum;}
	inline eType Type() const { return m_Type;}

	bool XmlWrite(std::ostream* out);

	bool Export_TA_Database(std::ostream* out, const char* header, char sep = '\t');
	bool Export_TA_MetaDonnees(std::ostream* out);
};

//-----------------------------------------------------------------------------
// Classe XSensor : gestion des cameras numeriques et argentiques
//-----------------------------------------------------------------------------
class XSensor{
public:
	// Orientation du capteur par rapport au sens de vol
	enum eOrientation { N2S, S2N, E2W, W2E};
	enum eRawFormat { 
		FORMAT_UNDEF, 
		FORMAT_RAW12_IGN, 
		FORMAT_MONO8,
		FORMAT_RGB8,
		FORMAT_RAW8_RGGB, 
		FORMAT_RAW8_GBRG, 
		FORMAT_RAW8_GRBG, 
		FORMAT_RAW8_BGGR, 
		FORMAT_MONO16,
		FORMAT_RGB16,
		FORMAT_RAW16_RGGB, 
		FORMAT_RAW16_GBRG, 
		FORMAT_RAW16_GRBG, 
		FORMAT_RAW16_BGGR, 
		FORMAT_NEF,
		FORMAT_JPEG
	};
protected:
	std::string	m_strName;		// Nom du capteur
	std::string	m_strObjectif;	// Nom de l'objectif
	std::string	m_strFile;		// Nom du fichier
	std::string	m_strFileOrigine;// Nom du fichier original
	std::string m_strOrigine;	// Origine du capteur
	std::string	m_strDate;		// Date de l'etalonnage
	std::string m_strSerial;	// Identifiant unique : à la fois de l'appareil, de l'objectif et de la calibration

	bool	m_bArgentique;		// Indique que l'on a un capteur argentique

	XRect m_Rect;				// Zone utile du capteur
	XRect m_Dark;				// Zone de calcul du courant d'obscurite
	std::vector<XDefect*>			m_Defect;		// Tableau des defauts
	std::vector<XColorInfo*>	m_Color;		// Tableau des informations sur les couleurs
	std::vector<XFiducial*>		m_Fiducial;	// Tableau des reperes de fond de chambre

	double	m_dPixelSize;	// Taille du pixel
	XPt3D		m_Focal;			// Focale exprimee en coordonnees image
	XPt2D		m_PPS;				// Point de meilleure symmetrie exprime en coordonnees image
	double	m_dR3;				// Polynomes de distorsion
	double	m_dR5;
	double	m_dR7;

	eOrientation	m_Orientation;

	// Gestion de la distortion
	bool		m_bDistoReady;	// Indique que les parametres internes de calcul sont prets
	XPt2D*	m_DistoValue;		// Tableau X : rayon, Y : rayon avec distortion
	uint16	m_nNbDistoValue;// Nombre de valeur dans le tableau
	XPt2D		m_PPScli;				// PPS exprime dans le repere cliche

	void UnloadDistorsion();	// Preparation des parametres internes de calcul

	double	m_dScanW;	// Largeur (en %) de la largeur de la bande noire de scan

	//chargement de la distorsion en mémoire
	XDistoImageData* m_distoData;

	eRawFormat m_rawFormat;
	XError* m_error;


public:
	//méthodes a utiliser avec une distorsion radiale
	XPt2D SubDistorsion(XPt2D A); // Passage image avec distortion -> image sans distortion
	XPt2D AddDistorsion(XPt2D A); // Passage cliche sans distortion -> cliche avec distortion


public:
	XSensor(std::string origine ="");
	XSensor(const XSensor& S);
	virtual ~XSensor();
	XSensor& operator=(const XSensor& S);

	void Error(XError* error){ m_error = error;;}
	XError* Error(){return m_error;}

	std::string Filename() const { return m_strFile;}
	std::string FilenameOrigine() const { return m_strFileOrigine;}
	std::string Name() const { return m_strName;}
	std::string Origine() const { return m_strOrigine;}
	std::string Objectif() const { return m_strObjectif;}
	std::string Date() const { return m_strDate;}
	std::string Serie() const { return m_strSerial;}
	uint32 SerieNum(); //convertion du numéro de série string en uint32

	void Filename(std::string File) { m_strFile = File;}
	void FilenameOrigine(std::string File) { m_strFileOrigine = File;}
	void Name(std::string Name) { m_strName = Name;}
	void Objectif(std::string Name) { m_strObjectif = Name;}
	void Origine(std::string Origine);
	void Date(std::string Date) { m_strDate = Date;}
	void Serie(std::string Serie) { m_strSerial = Serie;}

	inline uint32 Width()  const { return m_Rect.W;}	// Largeur (en pixels) utile
	inline uint32 Height() const { return m_Rect.H;}	// Hauteur (en pixels) utile

	XRect GetUsefullRect() { return m_Rect;}
	void SetUsefullRect(XRect Rect) {m_Rect = Rect;}

	XRect GetDarkRect() { return m_Dark;}
	void SetDarkRect(XRect Dark) {m_Dark = Dark;}

	inline double ScanW() { return m_dScanW;}
	void ScanW(double val) {m_dScanW = val;}

	inline bool Argentique() { return m_bArgentique;}
	void Argentique(bool flag) { m_bArgentique = flag;}

	uint32 NbDefect() const { return (uint32)m_Defect.size();}
	XDefect* GetDefect(uint32 num) { return(num < NbDefect())? m_Defect[num] : NULL;}
	void AddDefects(std::vector<XDefect*> VectDef);
	void DeleteDefects();	// Destruction de la liste des defauts

	uint32 NbColor() const { return (uint32)m_Color.size();}
	XColorInfo* GetColor(uint32 id);
	void AddColors(std::vector<XColorInfo*> VectCol);
	void DeleteColors();	// Destruction de la liste des informations sur les couleurs
	double GetChannelFactor(uint32 id);

	uint32 NbFiducial() const { return (uint32)m_Fiducial.size();}
	XFiducial* GetFiducial(uint32 num) { return(num < NbFiducial())? m_Fiducial[num] : NULL;}
	void AddFiducials(std::vector<XFiducial*> VectFid);
	void DeleteFiducials();	// Destruction de la liste des reperes

	void SetUsefullOrigin();

	inline double PixelSize() { return m_dPixelSize;}
	XPt3D	Focal() { return m_Focal;}
	XPt2D PPS() { return m_PPS;}

	void PixelSize(double size) {m_dPixelSize = size;}
	void Focal(XPt3D Focal) {m_Focal = Focal;}
	void PPS(XPt2D PPS) {m_PPS = PPS;}

	double Ouverture_W(bool applyDistorsion = false);
	double Ouverture_H(bool applyDistorsion = false);
	double Ouverture_Horizontale(bool applyDistorsion = false);
	double Ouverture_Verticale(bool applyDistorsion = false);

	XPt3D DistoPolynome(){return XPt3D(m_dR3,m_dR5,m_dR7);}
	void Distortion(double& r3, double& r5, double& r7)
													{ r3 = m_dR3; r5 = m_dR5; r7 = m_dR7;}
													
	XPt2D CorrigeDistorsion(XPt2D A);

	//Gestion d'un capteur avec grilles de distorsion
	bool LoadDistorsion(XError* error = NULL);	// Preparation des parametres internes de calcul

	bool TerrainToPhoto(double xIn, double yIn, double &xOut, double &yOut);
	bool PhotoToTerrain(double xIn, double yIn, double &xOut, double &yOut);
	bool SetDistoPolynome(double r3, double r5, double r7){
		m_dR3 = r3;  m_dR5 = r5; m_dR7 = r7;
		return LoadDistorsion();
	}
	bool DistoReady() {return m_bDistoReady;}

	// Gestion de l'orientation du capteur par rapport au sens de vol
	inline eOrientation Orientation() { return m_Orientation;}
	void Orientation(eOrientation ori) { m_Orientation = ori;}

	uint32 NadirSup();
	uint32 NadirInf();

	//photogrammétrie
	XPt2D Cli2Ima(XPt2D P);
    XPt3D Ima2Cli(XPt2D P);
    XPt2D Pk12Ima(XPt2D P);
	XPt3D Ima2Pk1(XPt2D P);

	uint32 FlightW();	// Largeur dans le sens du vol
	uint32 FlightH();	// Hauteur dans le sens du vol
	double  FlightFocal();//Position du point focal transversalement au repère photogrammétrique

	virtual bool XmlWrite(std::ostream* out);
	virtual bool XmlImageWrite(std::ostream* out);

	int CodeDistorsionOri();

	//Ecriture de la calibration au format cam pour comp3D
	virtual bool WriteCamComp3D(std::ostream* out);

	//Ecriture de la calibration au format dat pour TopAero
	bool WriteCamTopAero(std::ostream* out);

	//Matrice rotation correspondant au champ orientation (0,1,2,3)
	XMat3D MatOrientationCapteur();

	std::string DescriptionTexte();

	//chargement du Flatfield
	bool LoadFlatField(bool doWhiteBalance);


	eRawFormat RawFormat(){return m_rawFormat;};
	void RawFormat(eRawFormat format){m_rawFormat = format;}
	int BayerPattern();

	std::string RawFormatToString(eRawFormat format);
	eRawFormat StringToRawFormat(std::string strFormat);

	bool CheckAuxilliaryData();//vérifie la disponibilité des données auxilliaires

};

#endif //_XSENSOR_H
