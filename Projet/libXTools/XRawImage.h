//JP Papelard  - Erwann Houzay SR MATIS
#ifndef _X_RAW_IMAGE_H_
#define _X_RAW_IMAGE_H_

#include <string>
#include <vector>
#include "XError.h"
#include "XImageLoader.h"

class XRawImage
{
protected:
	std::string m_strFileName;
	XError* m_error;

protected:
	unsigned int	W;		// Largeur
	unsigned int	H;		// Hauteur
	unsigned char*	Pix;	// Tableau des pixels (eventuellement vide)

	unsigned int	Bps; //bit per sample
	unsigned int	NbChannel; //nombre de cannaux

	static std::vector<sXImageWriterType> m_ptrWriter;
	static std::vector<sXImageLoaderType> m_ptrLoader;

public:
	XRawImage(XError* error = NULL);
	XRawImage(const XRawImage& M);

	XRawImage(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel, XError* error);
	virtual ~XRawImage();

	std::string FileName(){return m_strFileName;}
	void FileName(std::string val);

	XError* Error(){return m_error;}
	void Error(XError* err){m_error = err;}

	bool Allocate(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel);
	bool SetDimension(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel);
	bool Allocate();
	void DeAllocate();
	bool IsLoaded();
	bool SOUS_ECH_BOURRIN(XRawImage *src, int ss);

	unsigned int	Width(){return	W;}
	unsigned int	Height(){return H;}

	unsigned int	BPS(){return Bps;}//8 , 16
	unsigned int	NChannel(){return NbChannel;}

	unsigned long LineW(){return W * NbChannel;}
	unsigned long LineSize(){return LineW() * Bps/8;}
	unsigned long RawSize(){return H * LineSize();}
	unsigned long PixelSize(){return NbChannel * Bps/8;}

	unsigned long NumPix(){return W * H ;}
	unsigned long NbPixels(){return W * H * NbChannel;}

	//Loader
	static void RegisterLoader(XImageLoaderBase * loader,const std::string &ext);
	static void RegisterWriter(XImageWriterBase * writer,const std::string &ext);
	static void FreeAllLoader();
	static void FreeAllWriter();

	bool ReadHeader(const char * filename);
	bool ReadFile(const char * filename);
	bool WriteFile(const char * filename,std::vector<unsigned int>  *options=NULL);

	//Swap the input buffer
	static void Swap16(unsigned short * Pix,unsigned long nbPix);
	void RAW16_to_RAW8(unsigned short * src, unsigned char * dest, unsigned long nbPix);

	///convertion function  used for AVT camera do not remove....
	static void RGB16_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels); 		
	static void MONO16_to_MONO8(unsigned char *src, unsigned char *dest, int NumPixels, unsigned int bits);
	static void YUV444_to_RGB8(unsigned char *src, unsigned char *dest, int width, int height);
	static void YUV422_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height, int byte_order);
	static void YUV411_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height);
	static void MONO16_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height, int bits);
	static void MONO8_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height);


	//12bits
	static void RGB12_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels);
	static void MONO12_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels);
	static void MONO12_to_MONO8(unsigned char * src, unsigned char * dest, int width, int height);
	static void RAW12_to_RAW16(unsigned char * src, unsigned char * dest, int width, int height);
	static void RAW12_to_ViewerRGB8(unsigned char * src, unsigned char * dest, int width, int height);

	//access aux pixels
	unsigned char* Pixels(){return Pix;}
	unsigned char* Col(unsigned int col, unsigned int lig );
	unsigned char* Line(unsigned int lig);
	unsigned char* GetPix(unsigned int col, unsigned int lig );//
	unsigned char*	PixValMin();	
	unsigned char*	PixValMax();	
	//acces aux pixels par des coordonées normalisées (0 --> 1)
	unsigned char* GetPixTexture(double x, double y);

	//conversion interne du buffer
	bool ConvertRAW12_to_RAW16();
	bool ConvertRGB8_to_BMP();
	bool Convert16_to_8();
	bool ConvertRGB8_to_Mono8();
	bool ConvertRGB_to_Mono();
	bool TranslateBufferRing(int nbPixeldecal);
	bool TranslateBufferRingLeft(uint16 nbPixeldecal);
	bool TranslateBufferRingRight(uint16 nbPixeldecal);
	bool Swap();
	bool IncreaseBufferDown(uint16 new_Height);
	bool RGB2BGR();
	bool Fill0Value();
};

#endif

