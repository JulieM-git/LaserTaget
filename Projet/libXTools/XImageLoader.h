//Erwann Houzay -SR-MATIS

#ifndef _X_IMAGE_LOADER_H_
#define _X_IMAGE_LOADER_H_

#include <string>
#include <vector>
#include "XError.h"

class XImageWriterBase;
class XImageLoaderBase;

class XRawImage;

typedef struct sXImageWriterTypeStruct
{
	std::string extension;
	XImageWriterBase * loader;
}sXImageWriterType;


typedef struct sXImageLoaderTypeStruct
{
	std::string extension;
	XImageLoaderBase * loader;
}sXImageLoaderType;


class XImageWriterBase
{
protected:
	XError* m_error;
public:
	XImageWriterBase(XError* error){m_error = error;}

	virtual bool Write(XRawImage * image, const char * filename,std::vector<unsigned int>  *options=NULL)=0;
};

class XImageLoaderBase
{
protected:
	XError* m_error;
public:
	XImageLoaderBase(XError* error){m_error = error;}

	virtual bool ReadHeader(XRawImage * image, const char * filename)=0;//charge les dimensions de l'image allouer ni charger le buffer
	virtual bool Read(XRawImage * image, const char * filename)=0;

};


#endif
