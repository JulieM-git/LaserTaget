#include "XArchiGeorefXML.h"

#include "XArchiGeoref.h"
#include "XArchiXMLTools.h"

namespace XArchiXML
{
		//-------------------------------------------------------------------------
	bool XArchiGeoref_LoadFromNode(XArchiGeoref* georef, TiXmlNode* node)
	{
		georef->Translation(XPt3D_LoadFromNode(node));
		georef->Rotation(XRotation_LoadFromNode(node));
		return true;
	}
	
}