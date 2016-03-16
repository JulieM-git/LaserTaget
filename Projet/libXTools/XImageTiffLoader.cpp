#include "XImageTiffLoader.h"
#include "XRawImage.h"

#include "tiffio.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XImageTiffLoader::XImageTiffLoader(XError* error):XImageLoaderBase(error)
{
}
//-----------------------------------------------------------------------------
bool XImageTiffLoader::ReadHeader(XRawImage * image, const char * filename)//charge les dimensions de l'image allouer ni charger le buffer
{
	TiffInfo TifInfo;
	if(!ReadInfo(filename,&TifInfo))
		return XErrorError(m_error,__FUNCTION__," Erreur de lecture des caractéristiques de l'image ",filename);
	return image->SetDimension(TifInfo.w,TifInfo.h,TifInfo.bsp,TifInfo.NumChannel) ;
}
//-----------------------------------------------------------------------------
bool XImageTiffLoader::ReadInfo(const char * filename, TiffInfo* info)
{
	TIFF *TiffImage;

	if((TiffImage = TIFFOpen(filename, "r")) == NULL)
		return XErrorError(m_error,__FUNCTION__,"Failed to open tiff image",filename);

	if(TIFFGetField(TiffImage, TIFFTAG_IMAGEWIDTH, &info->w) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_IMAGEWIDTH",filename);
	
	if(TIFFGetField(TiffImage, TIFFTAG_IMAGELENGTH, &info->h) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_IMAGELENGTH",filename);
	
	if(TIFFGetField(TiffImage, TIFFTAG_BITSPERSAMPLE, &info->bsp) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_BITSPERSAMPLE",filename);
		
	if(TIFFGetField(TiffImage, TIFFTAG_SAMPLESPERPIXEL, &info->NumChannel) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_SAMPLESPERPIXEL",filename);

	TIFFClose(TiffImage);
	return true;
}
//-----------------------------------------------------------------------------
bool XImageTiffLoader::Read(XRawImage *image, const char * filename)
{
	TIFF *TiffImage;

	tsize_t stripSize;

	int compression=COMPRESSION_NONE;

	int planar_conf=PLANARCONFIG_CONTIG;
	int fill_order=FILLORDER_LSB2MSB;
	int row_per_strip=0;
	
	int w = 0,h=0, bsp=8, NumChannel=1;

	if((TiffImage = TIFFOpen(filename, "r")) == NULL)
		return XErrorError(m_error,__FUNCTION__,"Failed to open tiff image",filename);

	if(TIFFGetField(TiffImage, TIFFTAG_IMAGEWIDTH, &w) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_IMAGEWIDTH",filename);
	
	if(TIFFGetField(TiffImage, TIFFTAG_IMAGELENGTH, &h) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_IMAGELENGTH",filename);
	
	if(TIFFGetField(TiffImage, TIFFTAG_BITSPERSAMPLE, &bsp) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_BITSPERSAMPLE",filename);
	
	NumChannel=0;
	if(TIFFGetField(TiffImage, TIFFTAG_SAMPLESPERPIXEL, &NumChannel) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_SAMPLESPERPIXEL",filename);

	/*
	if(NumChannel!=1 && NumChannel!=3)
	{
		unsigned int Photometric;
		TIFFGetField(TiffImage, TIFFTAG_PHOTOMETRIC, &Photometric);
		
		if(Photometric == PHOTOMETRIC_MINISBLACK)
			NumChannel=1;

		if(Photometric == PHOTOMETRIC_RGB)
			NumChannel=3;


	}
	*/

	TIFFGetField(TiffImage, TIFFTAG_FILLORDER, &fill_order);

	if(TIFFGetField(TiffImage, TIFFTAG_ROWSPERSTRIP, &row_per_strip) == 0)
		return XErrorError(m_error,__FUNCTION__,"Failed to read TIFFTAG_ROWSPERSTRIP",filename);
	
	//On regarde la compression
	TIFFGetField(TiffImage, TIFFTAG_COMPRESSION, &compression);
	TIFFGetField(TiffImage, TIFFTAG_PLANARCONFIG, &planar_conf);
	
	stripSize = TIFFStripSize (TiffImage);
	
	if(compression!=COMPRESSION_NONE && compression!=COMPRESSION_DEFLATE && compression!=COMPRESSION_LZW)
		return XErrorError(m_error,__FUNCTION__,"Unsupported compresssion",filename);

	if(planar_conf!=PLANARCONFIG_SEPARATE && planar_conf!=PLANARCONFIG_CONTIG)
		return XErrorError(m_error,__FUNCTION__,"Unsupported storage mode",filename);

	if(!image->Allocate(w,h, bsp, NumChannel))
		return XErrorError(m_error,__FUNCTION__,"mémoire insuffisante",filename);

	unsigned char * buf = image->Pixels();

	if(compression==COMPRESSION_DEFLATE || compression==COMPRESSION_LZW)
	{
		if(planar_conf==PLANARCONFIG_CONTIG)
		{
			for(int i=0; i<h; i+= row_per_strip)
				if(TIFFReadEncodedStrip (TiffImage, TIFFComputeStrip(TiffImage,i,0),buf+(i/row_per_strip)*stripSize,stripSize) == -1)
					return XErrorError(m_error,__FUNCTION__,"Failed to read image content",filename);
		}
		if(planar_conf==PLANARCONFIG_SEPARATE)
		{
			for(int j=0;j<NumChannel;j++)
				for(int i=0; i<h; i+=row_per_strip)
					if(TIFFReadEncodedStrip (TiffImage, TIFFComputeStrip(TiffImage,i,j),buf+(i/row_per_strip)*stripSize,stripSize) == -1)
						return XErrorError(m_error,__FUNCTION__,"Failed to read image content",filename);
		}
	}
	
	if(compression==COMPRESSION_NONE)
	{
		if(planar_conf==PLANARCONFIG_CONTIG)
		{
			for(int i=0;i<h;i+=row_per_strip)
				if(TIFFReadRawStrip (TiffImage, TIFFComputeStrip(TiffImage,i,0),buf+(i/row_per_strip)*stripSize,stripSize) == -1)
					return XErrorError(m_error,__FUNCTION__,"Failed to read image content",filename);
		}
		if(planar_conf==PLANARCONFIG_SEPARATE)
		{
			for(int j=0;j<NumChannel;j++)
				for(int i=0;i<h;i+=row_per_strip)
					if(TIFFReadRawStrip (TiffImage, TIFFComputeStrip(TiffImage,i,j),buf+(i/row_per_strip)*stripSize,stripSize) == -1)
						return XErrorError(m_error,__FUNCTION__,"Failed to read image content",filename);
		}
	}

	TIFFClose(TiffImage);
	return true;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
XImageTiffWriter::XImageTiffWriter(XError* error):XImageWriterBase(error)
{
}
//-----------------------------------------------------------------------------
bool XImageTiffWriter::Write(XRawImage *image, const char * filename,std::vector<unsigned int>  *options)
{
	if(strlen(filename) == 0)
		return XErrorError(m_error,__FUNCTION__,"Le nom de fichier à créer est vide !");

	bool encode = false;
	bool separate = false;
	bool invertEndianess  = false;

	if(options!=NULL)
	{
		for(std::vector<unsigned int >::iterator it =options->begin();it!=options->end();it++)
		{
			if((*it) == TIFF_WRITER_OPTIONS_COMPRESS_DEFLATE)
				encode = true;
			if((*it) == TIFF_WRITER_OPTIONS_STORE_MODE_SEPARATE)
				separate = true;
			if((*it) == TIFF_INVERTED_ENDIANS)
				invertEndianess = true;
			
		}
	}

	TIFF *TiffImage;
	if((TiffImage = TIFFOpen(filename, "wl")) == NULL)
		return XErrorError(m_error,__FUNCTION__,"Impossible d'ouvrir le fichier de sortie ",filename);

	TIFFSetField(TiffImage, TIFFTAG_BITSPERSAMPLE, image->BPS());
	TIFFSetField(TiffImage, TIFFTAG_SAMPLESPERPIXEL, image->NChannel());
	TIFFSetField(TiffImage, TIFFTAG_IMAGEWIDTH, image->Width());
	TIFFSetField(TiffImage, TIFFTAG_IMAGELENGTH, image->Height() );
	TIFFSetField(TiffImage, TIFFTAG_ROWSPERSTRIP, image->Height());

	if(invertEndianess)
		TIFFSetField(TiffImage, TIFFTAG_FILLORDER,FILLORDER_LSB2MSB );
	else
		TIFFSetField(TiffImage, TIFFTAG_FILLORDER,FILLORDER_MSB2LSB );
	
	!separate?TIFFSetField(TiffImage, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG):TIFFSetField(TiffImage, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);

	TIFFSetField(TiffImage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	if(image->NChannel() == 1)
		TIFFSetField(TiffImage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	if(image->NChannel() == 3)
		TIFFSetField(TiffImage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	!encode?TIFFSetField(TiffImage, TIFFTAG_COMPRESSION, COMPRESSION_NONE):TIFFSetField(TiffImage, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	
	if(!encode)
	{
		if(!separate)
		{
			if(TIFFWriteRawStrip(TiffImage,0, image->Pixels(),image->RawSize())<0)			
			{
				TIFFClose(TiffImage);
				return XErrorError(m_error,__FUNCTION__,"Failed to write image content inside output file",filename);
			}
		}else
		{
			unsigned char *temp=image->Pixels();
			long strip_size = image->RawSize()/image->NChannel();
			for(unsigned int i=0;i<image->NChannel();i++)
			{
				if(TIFFWriteRawStrip(TiffImage,TIFFComputeStrip(TiffImage,0,i), temp,strip_size)<0)
				{
						TIFFClose(TiffImage);
						return XErrorError(m_error,__FUNCTION__,"Failed to write image content inside output file",filename);
				}

				if(i!=image->NChannel()-1)
					temp+=strip_size;
			}
		}
	}else
	{
		if(!separate)
		{
			if(TIFFWriteEncodedStrip(TiffImage,0, image->Pixels(),image->RawSize())<0)
			{
				TIFFClose(TiffImage);
				return XErrorError(m_error,__FUNCTION__,"Failed to write image content inside output file",filename);
			}
		}else
		{
			unsigned char *temp=image->Pixels();
			long strip_size = image->RawSize()/image->NChannel();
			for(unsigned int i=0;i<image->NChannel();i++)
			{
				if(TIFFWriteEncodedStrip(TiffImage,TIFFComputeStrip(TiffImage,0,i), temp,strip_size)<0)
				{
						TIFFClose(TiffImage);
						return XErrorError(m_error,__FUNCTION__,"Failed to write image content inside output file",filename);
				}
				if(i!=image->NChannel()-1)
					temp+=strip_size;
			}
		}

	}
	TIFFClose(TiffImage);

	return true;
}
//-----------------------------------------------------------------------------
bool XImageTiffWriter::Save(const char *filename,unsigned char *buf,unsigned int width,unsigned int height,unsigned int bit_per_sample,unsigned int num_channel)
{
	TIFF *m_pimage;
	if((m_pimage = TIFFOpen(filename, "wl")) == NULL)
		return XErrorError(m_error,__FUNCTION__,"Erreur de création du fichier de sortie ",filename);

	TIFFSetField(m_pimage, TIFFTAG_BITSPERSAMPLE, bit_per_sample);
	TIFFSetField(m_pimage, TIFFTAG_SAMPLESPERPIXEL, num_channel);
	TIFFSetField(m_pimage, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(m_pimage, TIFFTAG_IMAGELENGTH, height );
	TIFFSetField(m_pimage, TIFFTAG_ROWSPERSTRIP, height);
//	TIFFSetField(m_pimage, TIFFTAG_FILLORDER, FILLORDER_LSB2MSB);

	TIFFSetField(m_pimage, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	if(num_channel ==3)
		TIFFSetField(m_pimage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	if(num_channel ==1)
		TIFFSetField(m_pimage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

	TIFFSetField(m_pimage, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

	float bit = (float)((float)bit_per_sample/8.0);

	long size=(long) ceil(bit*num_channel*width*height);
	
	if(TIFFWriteRawStrip(m_pimage,TIFFComputeStrip(m_pimage,0,0), buf,size)<0)
	{
			TIFFClose(m_pimage);
			return XErrorError(m_error,__FUNCTION__,"Erreur TIFFWriteRawStrip ",filename);
	}
	
	TIFFClose(m_pimage);
	return true;
}


