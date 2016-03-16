#include "XSensorXML.h"

#include "XSensor.h"
#include "XPath.h"
#include "XArchiXMLBaseTools.h"
#include "XArchiXMLTools.h"
#include "XArchiXMLException.h"
#include "tinyxml.h"

namespace XArchiXML
{

	const std::string TAG_sensor_origine =		"origine";
	const std::string TAG_sensor_serie =		"serial-number";

	const std::string TAG_sensor_usefull_frame ="usefull-frame";
	const std::string TAG_sensor_dark_frame =	"dark-frame";
	const std::string TAG_sensor_focal =		"focal";
	const std::string TAG_sensor_defect =		"defect";
	const std::string TAG_sensor_color =		"color_info";
	const std::string TAG_sensor_distortion =    "distortion";
	const std::string TAG_sensor_doublegrid =    "doublegrid";
	const std::string TAG_sensor_grid_directe =    "grid_directe";
	const std::string TAG_sensor_grid_inverse =    "grid_inverse";


	const std::string TAG_sensor_pixelsize =    "pixel_size";
	const std::string TAG_sensor_orientation =   "orientation";
	const std::string TAG_sensor_fileOrigine =   "file_origine";

	const std::string TAG_sensor_rawformat =   "raw-format";
	const std::string TAG_sensor_flatfield =   "flatfield";

//-----------------------------------------------------------------------------
bool XColorInfo_LoadFromNode(XColorInfo* color, TiXmlNode* node)
{
	color->Id(ReadNodeAsUint32(node,"channel_id",color->Id()));
	color->Name(ReadNodeAsString(node,"channel_name"));
	color->Factor(ReadNodeAsDouble(node,"factor",color->Factor()));
	color->R(ReadNodeAsDouble(node,"red",color->R()));
	color->G(ReadNodeAsDouble(node,"green",color->G()));
	color->B(ReadNodeAsDouble(node,"blue",color->B()));
	return true;
}

//-------------------------------------------------------------------------
bool XDefect_LoadFromNode(XDefect* defect, TiXmlNode* node)
{
	std::string type = ReadNodeAsString(node,"type");
	defect->SetType(XDefect::eNull);

	if (type == "HotPix")
		defect->SetType(XDefect::eTemperature);
	if (type == "Gap")
		defect->SetType(XDefect::eGap);
	if (type == "Cluster")
		defect->SetType(XDefect::eCluster);
	if (defect->Type() == XDefect::eNull)
		return false;

	XRect rect;
	if(! XRect_LoadFromNode(&rect,node))
		return false;

	defect->SetRect(rect);
	defect->SetValue(ReadNodeAsUint32(node,"value"));
	return true;
}

//-------------------------------------------------------------------------
bool XSensor_LoadFileXml(XSensor* sensor, std::string filename)
{
	char message[1024];
	//validation du formalisme XML
	TiXmlDocument doc( filename.c_str() );
	if (!doc.LoadFile() )
	{
		sprintf( message,"Format XML non conforme pour %s. Error='%s'\n",filename.c_str(), doc.ErrorDesc() );
		throw(XmlException(message,XmlException::eBadFormat));
		return false;
	}
		
	sensor->Filename(filename);
	TiXmlElement* root = doc.RootElement();

	std::string strRoot = std::string( root->Value() );
	if(strRoot == "ArchiImageSensor")
	{
		TiXmlNode* sens = FindAssertNode(root,std::string("sensor"));
		return XSensor_LoadFromNode(sensor, sens);
	}
	if(strRoot == "sensor")
		return XSensor_LoadFromNode(sensor, root);

	if(strRoot == "ExportAPERO")
		return XSensor_LoadFromNodeApero(sensor, root);

	std::string ident = filename + '\\' + strRoot;
	throw(XmlException(ident.c_str(),XmlException::eRacine));
}
//-------------------------------------------------------------------------
 bool XSensor_LoadFromNode(XSensor* sensor, TiXmlNode* node)
{
	sensor->Origine(ReadAssertNodeAsString(node,TAG_sensor_origine));
	sensor->Serie(ReadNodeAsString(node,TAG_sensor_serie));

	TiXmlNode* usefullFrame = FindAssertNode(node,TAG_sensor_usefull_frame);
	sensor->SetUsefullRect(XRect_LoadFromNode(usefullFrame));

	TiXmlNode* darkFrame = FindAssertNode(node,TAG_sensor_dark_frame);
	sensor->SetDarkRect(XRect_LoadFromNode(darkFrame));

	// Lecture des champs d'information
	sensor->Name(ReadNodeAsString(node,"name"));
	sensor->Objectif(ReadNodeAsString(node,"objectif"));
	sensor->Date (ReadNodeAsString(node,"calibration-date"));

	sensor->Argentique(ReadNodeAsBool(node,"argentique",sensor->Argentique()));
	// Le champ argentique n'est pas forcement rempli -> verification avec l'origine du capteur
	if ((sensor->Origine().compare("Argentique")==0) || (sensor->Origine().compare("Argentique_tournee")==0))
		sensor->Argentique(true);

	sensor->ScanW(ReadNodeAsDouble(node,"scan_width"));

	// Lecture des defauts
	sensor->DeleteDefects();
	std::vector<XDefect*> vecDefects;
	TiXmlHandle hdl(node);
    TiXmlElement* defect =  hdl.FirstChild(TAG_sensor_defect.c_str()).ToElement();
	while(defect)
	{
		XDefect* newDefect = new XDefect;
		if( XDefect_LoadFromNode(newDefect,defect))
			vecDefects.push_back(newDefect);

        defect = defect->NextSiblingElement(TAG_sensor_defect.c_str());
	}
	if (vecDefects.size() > 0)
		sensor->AddDefects(vecDefects);

	// Lecture des informations sur les couleurs
	sensor->DeleteColors();
	std::vector<XColorInfo*> vecColor;
    
   TiXmlElement* color =  hdl.FirstChild(TAG_sensor_color.c_str()).ToElement();
	while(color)
	{
		XColorInfo* newColor = new XColorInfo;
		if( XColorInfo_LoadFromNode(newColor,color))
			vecColor.push_back(newColor);
        color = color->NextSiblingElement(TAG_sensor_color.c_str());
	}
	if (vecColor.size() > 0)
		sensor->AddColors(vecColor);

	// Lecture des reperes de fond de chambre
	sensor->DeleteFiducials();
	//à coder si nécessaire !!!

	// Lecture du point focal
	TiXmlNode* focal = FindAssertNode(node,TAG_sensor_focal);
	sensor->Focal(XPt3D_LoadFromNode(focal));

	//Lecture des paramètres de distortion polynomiale
	TiXmlNode* disto = node->FirstChild(TAG_sensor_distortion.c_str());
	if(disto != NULL)
	{
		XPt2D pps;
		if(XPt2D_LoadFromNode(&pps,disto))
		{
			double r3=0,r5=0,r7=0;
			r3 = ReadNodeAsDouble(disto,"r3",r3);
			r5 = ReadNodeAsDouble(disto,"r5",r5);
			r7 = ReadNodeAsDouble(disto,"r7",r7);
			sensor->PPS( pps);
			if(!sensor->SetDistoPolynome(r3,r5,r7)){//cette fonction retourne un LoadDistorsion()
				throw(XmlException("Load Disto Polynome", XmlException::eBadData));
				return false ;
			}
		}
	}


	//taille du pixel
	sensor->PixelSize(ReadAssertNodeAsDouble(node,TAG_sensor_pixelsize));

	// Orientation du capteur
	sensor->Orientation((XSensor::eOrientation)ReadNodeAsInt(node,TAG_sensor_orientation.c_str(),0));

	sensor->FilenameOrigine(ReadNodeAsString(node,TAG_sensor_fileOrigine.c_str()));

	//nouveau *************
	sensor->RawFormat(sensor->StringToRawFormat(ReadNodeAsString(node,TAG_sensor_rawformat)));

	return true;
}
//-------------------------------------------------------------------------
/*<ExportAPERO>
     <CalibrationInternConique>
          <KnownConv>eConvApero_DistM2C</KnownConv>
          <PP>1039.02982688905695 1026.08035572659674</PP>
          <F>1127.33404238428875</F>
          <SzIm>2048 2048</SzIm>
          <CalibDistortion>
               <ModRad>
                    <CDist>1042.79129219386891 1023.34369425995885</CDist>
                    <CoeffDist>-1.40719740034123929e-07</CoeffDist>
                    <CoeffDist>3.65320414724865432e-14</CoeffDist>
                    <CoeffDist>-4.65586976953662635e-21</CoeffDist>
               </ModRad>
          </CalibDistortion>
     </CalibrationInternConique>
</ExportAPERO>*/
 bool XSensor_LoadFromNodeApero(XSensor* sensor, TiXmlNode* node)
{
	sensor->FilenameOrigine(InitFileName(node));
	XPath p;
	sensor->Name(p.NameNoExt(InitFileName(node).c_str())+"_micmac");
	sensor->Origine(sensor->Name());
	sensor->Argentique(false);

	TiXmlNode* calincon =  FindAssertNode(node,"CalibrationInternConique");
	XPt2D frame= XPt2D_LoadLineInNode(FindAssertNode(calincon,"SzIm"));
	sensor->SetUsefullRect(XRect(0,0,frame.X,frame.Y));

	XPt3D focal= XPt2D_LoadLineInNode(FindAssertNode(calincon,"PP"));
	focal.Z = ReadAssertNodeAsDouble(calincon,"F");

	TiXmlNode* disto =  FindAssertNode(calincon,"CalibDistortion");
	TiXmlNode* modrad =  FindAssertNode(disto,"ModRad");
	
	XPt2D pps = XPt2D_LoadLineInNode(FindAssertNode(modrad,"CDist"));
	sensor->PPS( pps);

	TiXmlHandle hdl(modrad);
    TiXmlElement* coef =  hdl.FirstChild("CoeffDist").ToElement();
	std::vector<double> vecCoef;
	while(coef)
	{
		TiXmlElement *  elt = coef->ToElement ();
		const char* val = elt->GetText();
		double rx;
		sscanf(val, "%lf", &rx);
		vecCoef.push_back(rx);
        coef = coef->NextSiblingElement("CoeffDist");
	}
	if (vecCoef.size() != 3)
	{
		std::string ident = NodeGeneaologie(node) + "le modèle de distorsion n'est pas RadialStd " ;
		throw(XmlException(ident.c_str(), XmlException::eBadData));
	}
	if(!sensor->SetDistoPolynome(vecCoef[0],vecCoef[1],vecCoef[2]))//cette fonction retourne un LoadDistorsion()
		throw(XmlException("Load Disto Polynome", XmlException::eBadData));

	return true;
}

}
