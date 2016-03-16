//XSensorXML.h
#ifndef _X_SENSOR_XML_H_
#define _X_SENSOR_XML_H_

#include <string>
#include <iostream>

class XSensor;
class TiXmlNode;
class XGrille;
class XColorInfo;
class XDefect;
class XFiducial;
class XRawImageFlatField;
class XDeviceInfo;
class XChromaticData;

namespace XArchiXML
{
	extern bool XColorInfo_LoadFromNode(XColorInfo* color, TiXmlNode* node);
	extern bool XDefect_LoadFromNode(XDefect* defect, TiXmlNode* node);
	extern bool XSensor_LoadFileXml(XSensor* sensor, std::string filename);
	extern bool XSensor_LoadFromNode(XSensor* sensor, TiXmlNode* node);
	extern bool XSensor_LoadFromNodeApero(XSensor* sensor, TiXmlNode* node);
	extern bool XRawImageFlatField_LoadFromNode(XRawImageFlatField* FF, TiXmlNode* node);
	extern bool XDeviceInfo_LoadFromNode(XDeviceInfo* DI, TiXmlNode* node);
	extern bool XChromaticData_LoadFromNode(XChromaticData* CD, TiXmlNode* node);
}

#endif