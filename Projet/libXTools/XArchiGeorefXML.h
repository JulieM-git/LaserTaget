#ifndef _X_ARCHI_GEOREF_XML_H_
#define _X_ARCHI_GEOREF_XML_H_

#include <iostream>
class XArchiGeoref;
class TiXmlNode;

namespace XArchiXML
{
	bool XArchiGeoref_LoadFromNode(XArchiGeoref* georef, TiXmlNode* node);
}
#endif