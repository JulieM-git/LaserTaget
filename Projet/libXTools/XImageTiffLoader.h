#ifndef _X_IMAGE_TIFF_LOADER_H_
#define _X_IMAGE_TIFF_LOADER_H_

#include "XImageLoader.h"
#include <vector>
struct TiffInfo
{
	uint16 w;
	uint16 h;
	uint16 bsp;
	uint16 NumChannel;
};
class XImageTiffLoader:public XImageLoaderBase
{
public:
	XImageTiffLoader(XError* error);
	virtual bool ReadHeader(XRawImage * image, const char * filename);//charge les dimensions de l'image allouer ni charger le buffer
	virtual bool Read(XRawImage *image, const char * filename);

	bool ReadInfo(const char * filename, TiffInfo* info);

};

enum
{
	TIFF_WRITER_OPTIONS_COMPRESS_DEFLATE,		//Compression LZW
	TIFF_WRITER_OPTIONS_STORE_MODE_SEPARATE,	//Canaux séparés
	TIFF_INVERTED_ENDIANS
};

class XImageTiffWriter:public XImageWriterBase
{
public:
	XImageTiffWriter(XError* error);

	virtual bool Write(XRawImage *image, const char * filename,std::vector<unsigned int>  *options=NULL);
	bool Save(const char *filename,unsigned char *buf,unsigned int width,unsigned int height,unsigned int bit_per_sample,unsigned int num_channel);
};

#endif