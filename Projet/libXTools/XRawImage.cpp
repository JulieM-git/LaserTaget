#include "XRawImage.h"
#include "XPath.h"
#include "XStringTools.h"
#include "XImage.h"

//-----------------------------------------------------------------------------
enum 
{
	BYTE_ORDER_UYVY=0,
	BYTE_ORDER_YUYV
};

#define YUV2RGB(y, u, v, r, g, b) {\
			r = y + ((v*1436) >> 10);\
			g = y - ((u*352 + v*731) >> 10);\
			b = y + ((u*1814) >> 10);\
			r = r < 0 ? 0 : r;\
			g = g < 0 ? 0 : g;\
			b = b < 0 ? 0 : b;\
			r = r > 255 ? 255 : r;\
			g = g > 255 ? 255 : g;\
			b = b > 255 ? 255 : b; }



std::vector<sXImageWriterType> XRawImage::m_ptrWriter;
std::vector<sXImageLoaderType> XRawImage::m_ptrLoader;
//-----------------------------------------------------------------------------
XRawImage::XRawImage(XError* error)
{
	m_error = error;
	Pix = NULL; 
	W=0;
	H=0;
	Bps=0;
	NbChannel=0;
}


//-----------------------------------------------------------------------------
XRawImage::XRawImage(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel,XError* error)
{
	m_error = error;
	Pix = NULL; 
	W = w;
	H = h;
	Bps = bps;
	NbChannel = numChannel;
	Allocate();
}
//-----------------------------------------------------------------------------
XRawImage::~XRawImage()
{
	DeAllocate();
}
//-----------------------------------------------------------------------------
void XRawImage::FileName(std::string val)
{
	m_strFileName = val;
}
//-----------------------------------------------------------------------------
XRawImage::XRawImage(const XRawImage& M)
{
	m_error = M.m_error;
	Pix = NULL;
	
	W = M.W; 
	H = M.H;

	NbChannel = M.NbChannel;
	Bps =M.Bps;
	m_strFileName =M.m_strFileName;

	if (M.Pix != NULL) 
	{
		Allocate();
		memcpy(Pix, M.Pix,RawSize());
	}
}
//-----------------------------------------------------------------------------
bool XRawImage::Allocate(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel)
{
	if(!SetDimension(w, h, bps, numChannel))
		return false;
	return Allocate();
}
//-----------------------------------------------------------------------------
bool XRawImage::SetDimension(unsigned int w, unsigned int h, unsigned int bps, unsigned int numChannel)
{
	W = w;
	H = h;
	Bps = bps;
	NbChannel = numChannel;
	if((bps != 8)&&(bps != 12)&&(bps != 16))
		return XErrorError(m_error,__FUNCTION__,"valeur de bps non valide",m_strFileName.c_str());
	if((numChannel != 1)&&(numChannel != 3))
		return XErrorAlert(m_error,__FUNCTION__,"nombre de canaux non conventionnel",m_strFileName.c_str());//renvoie true
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::Allocate()
{
	DeAllocate();
	try
	{
		Pix = new unsigned char[RawSize()];
	}
	catch(...)
	{
		char message[1024];
		sprintf(message," Echec allocation de %.2lf Ko ",RawSize()/1024.);
		return XErrorError(m_error,__FUNCTION__,message,m_strFileName.c_str());
	}

	if(Pix == NULL)
		return XErrorError(m_error,__FUNCTION__," Erreur d'allocation ",m_strFileName.c_str());
	return true;
}
//-----------------------------------------------------------------------------
void XRawImage::DeAllocate()
{
	if(Pix!=NULL)
		delete [] Pix;

	Pix = NULL;
}
//-----------------------------------------------------------------------------
bool XRawImage::IsLoaded()
{
	if(Pix==NULL)
		return false; 
	return true;
}

//********************* accès aux pixels **************************** 
//-----------------------------------------------------------------------------
unsigned char* XRawImage::Col(unsigned int col, unsigned int lig )  
{
	if(Pix == NULL)
		return Pix;
	return &Pix[col*NbChannel+lig *LineW() ];
}
//-----------------------------------------------------------------------------
unsigned char* XRawImage::Line(unsigned int lig)  
{
	if(Pix == NULL)
		return Pix;
	return &Pix[lig*LineW()];
}
//-----------------------------------------------------------------------------
unsigned char* XRawImage::GetPix(unsigned int col, unsigned int lig )
{
	if(Pix == NULL)
		return Pix;
	return  &Pix[(lig*W+col)*NbChannel*Bps/8];
}
//-----------------------------------------------------------------------------
unsigned char*	XRawImage::PixValMin()//utilisable avec 3 canaux mais le min est resté sur le rouge
{
	if(Pix == NULL)
		return NULL;
	unsigned char* pix = Pixels();
	unsigned char* pixMin = pix;
	for(uint32 i=0; i< NumPix(); i++)
	{
		if(*pix < *pixMin)
			pixMin = pix;
		pix =  pix+NbChannel;
	}
	return  pixMin;
}	
//-----------------------------------------------------------------------------
unsigned char*	XRawImage::PixValMax()//utilisable avec 3 canaux mais le min est resté sur le rouge
{
	if(Pix == NULL)
		return NULL;
	unsigned char* pix = Pixels();
	unsigned char* pixMax = pix;
	for(uint32 i=0; i< NumPix(); i++)
	{
		if(*pix > *pixMax)
			pixMax = pix;
		pix =  pix+NbChannel;
	}
	return  pixMax;
}	

//acces aux pixels par des coordonées normalisées (0 --> 1)
//-----------------------------------------------------------------------------
unsigned char* XRawImage::GetPixTexture(double x, double y)
{
	if(Pix == NULL)
		return Pix;
	uint32 col =  x*W;
	col = XMin(W,col);
	uint32 lig = y*H;
	lig = XMin(H,lig);
	return &Pix[col*NbChannel + lig *LineW() ];
}
	
//****************** Image loading *******************************/
void XRawImage::RegisterLoader(XImageLoaderBase * loader,const std::string & ext)
{
	for(unsigned int i=0;i<m_ptrLoader.size();i++)
	{
		if(m_ptrLoader[i].loader == loader)
			return;
	}
	XStringTools st;
	sXImageLoaderType Loader;
	Loader.extension = st.lowercase(ext);
	Loader.loader = loader;

	m_ptrLoader.push_back(Loader);
}
//-----------------------------------------------------------------------------
void XRawImage::FreeAllLoader()
{
	for (std::vector<sXImageLoaderType>::iterator iter_L = m_ptrLoader.begin(); iter_L != m_ptrLoader.end(); iter_L++)
		delete (*iter_L).loader;
	m_ptrLoader.clear();
}
//-----------------------------------------------------------------------------
void XRawImage::RegisterWriter(XImageWriterBase * writer,const std::string &ext)
{
	for(unsigned int i=0;i<m_ptrWriter.size();i++)
	{
		if(m_ptrWriter[i].loader == writer)
			return;
	}

	XStringTools st;
	sXImageWriterType Writer;
	Writer.extension = st.lowercase(ext);
	Writer.loader = writer;

	m_ptrWriter.push_back(Writer);
}
//-----------------------------------------------------------------------------
void XRawImage::FreeAllWriter()
{
	for (std::vector<sXImageWriterType>::iterator iter_W = m_ptrWriter.begin(); iter_W != m_ptrWriter.end(); iter_W++)
		delete (*iter_W).loader;
	m_ptrWriter.clear();
}
//-----------------------------------------------------------------------------
bool XRawImage::ReadHeader(const char * filename)
{
	m_strFileName = "";
	XPath p; XStringTools st;
	std::string ext = st.lowercase(p.Extension(filename));
	if(ext.empty())
		return XErrorError(m_error,__FUNCTION__,"Fichier sans extension",filename);
	
	for(std::vector<sXImageLoaderType>::iterator it = m_ptrLoader.begin();it!=m_ptrLoader.end();it++)
	{
		if(ext == (*it).extension)
		{
			if(!(*it).loader->ReadHeader(this,filename))
				return false;
			m_strFileName = filename;
			return true;
		}
	}
	return  XErrorError(m_error,__FUNCTION__,"Extension de fichier non gérée",filename);;
}
//-----------------------------------------------------------------------------
bool XRawImage::ReadFile(const char * filename)
{
	m_strFileName = "";
	XPath p; XStringTools st;
	std::string ext = st.lowercase(p.Extension(filename));
	if(ext.empty())
		return XErrorError(m_error,__FUNCTION__,"Fichier sans extension",filename);
	
	for(std::vector<sXImageLoaderType>::iterator it = m_ptrLoader.begin();it!=m_ptrLoader.end();it++)
	{
		if(ext == (*it).extension)
		{
			if(!(*it).loader->Read(this,filename))
				return false;
			m_strFileName = filename;
			return true;
		}
	}
	return  XErrorError(m_error,__FUNCTION__,"Extension de fichier non gérée",filename);;
}
//-----------------------------------------------------------------------------
bool XRawImage::WriteFile(const char * filename,std::vector<unsigned int>  *options)
{
	XPath p; XStringTools st;
	std::string ext = st.lowercase(p.Extension(filename));
	if(ext.empty())
		return XErrorError(m_error,__FUNCTION__," Extension non spécifiée ",filename);
	
	for(std::vector<sXImageWriterType>::iterator it = m_ptrWriter.begin();it!=m_ptrWriter.end();it++)
	{
		if(ext == (*it).extension)
			return (*it).loader->Write(this,filename,options);
	
	}
	return XErrorError(m_error,__FUNCTION__," Extension non supportée ",filename);
}
//-----------------------------------------------------------------------------
//Byte swapping
void XRawImage::Swap16(unsigned short * Pix, unsigned long nbPix)
{
	unsigned short * buf=(unsigned short *)Pix;
	for(unsigned int i=0; i < nbPix; i++)
	{
		buf[i] = ((((unsigned short)buf[i])<< 8) & 0xFF00)  |
				((((unsigned short)buf[i])>> 8) & 0x00FF);
	}
}

//-----------------------------------------------------------------------------
//Sousech expédié
bool XRawImage::SOUS_ECH_BOURRIN(XRawImage  *src, int ss)
{
	//if((src->BPS() == 16)&&(src->NChannel() == 1))//raw 16bits
	//{
	//	XRawImage Rgb8(src->Width(),src->Height(),8,3,src->Error());
	//	MONO16_to_RGB8(src->Pixels(),Rgb8.Pixels(),src->Width(),src->Height(),src->BPS());
	//	if(!Rgb8.WriteFile("E:\\Test_Pike\\mno16torgb8.tif"))
	//		return false;
	//	src = &Rgb8;
	//}

	if(src->BPS() != 8)
		return XErrorError(m_error,__FUNCTION__," autre que 8 bits non implementé","");
	if(src->NChannel() != 3)
		return XErrorError(m_error,__FUNCTION__," autre que 3 canaux","");

	int w = src->Width();
	int h = src->Height();
	uint32 ssW = floor((double)w/(double)(2*ss));
	uint32 ssH = floor((double)h/(double)(2*ss));

	Allocate(ssW,ssH,src->BPS(),src->NChannel());

	unsigned char *line;	
	unsigned char *out = Pix;
	for(long i=0; i< h-2*ss; i+=2*ss)
	{		
		line=src->Pixels() + 3 * i * w;
		for(long j=0;j<w;j+=2*ss, line += 6*ss)
		{
			* out = *line;		out++;
			* out = *(line+1);	out++;
			* out = *(line+2);	out++;
		}
	}
        return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////
//Color conversion routines
//-----------------------------------------------------------------------------
void XRawImage::RGB16_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels)
{
  int i = ((NumPixels + ( NumPixels << 1 )) << 1)-1;
  int j = NumPixels + ( NumPixels << 1 ) -1;

  while (i >= 0) {
    i--;
    dest[j--]=src[i--];
    i--;
    dest[j--]=src[i--];
    i--;
    dest[j--]=src[i--];
  }
}
//-----------------------------------------------------------------------------
void XRawImage::MONO16_to_MONO8(unsigned char *src, unsigned char *dest, int NumPixels, unsigned int bits)
{
  int i = (NumPixels<<1)-1;
  int j = NumPixels-1;
  int y;

  while (i >= 0) {
    y = src[i--];
    dest[j--] = (y + (src[i--]<<8))>>(bits-8);
  }
}
//-----------------------------------------------------------------------------
void XRawImage::YUV444_to_RGB8(unsigned char *src, unsigned char *dest, int width, int height)
{
	int i = (width*height) + ( (width*height) << 1 ) -1;
	int j = (width*height) + ( (width*height) << 1 ) -1;
	int y, u, v;
	int r, g, b;

	while (i >= 0) 
	{
		v = (unsigned char) src[i--] - 128;
		y = (unsigned char) src[i--];
		u = (unsigned char) src[i--] - 128;
		YUV2RGB (y, u, v, r, g, b);
		dest[j--] = b;
		dest[j--] = g;
		dest[j--] = r;  
	}
}
//-----------------------------------------------------------------------------
void XRawImage::YUV422_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height, int byte_order)
{
  int i = ((width*height) << 1)-1;
  int j = (width*height) + ( (width*height) << 1 ) -1;
  int y0, y1, u, v;
  int r, g, b;

  switch (byte_order) 
  {
	case BYTE_ORDER_YUYV:
		while (i >= 0) 
		{
			v   = (unsigned char) src[i--] -128;
			y1  = (unsigned char) src[i--];
			u   = (unsigned char) src[i--] -128;
			y0  = (unsigned char) src[i--];
			YUV2RGB (y1, u, v, r, g, b);
			dest[j--] = b;
			dest[j--] = g;
			dest[j--] = r;
			YUV2RGB (y0, u, v, r, g, b);
			dest[j--] = b;
			dest[j--] = g;
			dest[j--] = r;
		}
		break;
	case BYTE_ORDER_UYVY:
		while (i >= 0) 
		{
			y1 = (unsigned char) src[i--];
			v  = (unsigned char) src[i--] - 128;
			y0 = (unsigned char) src[i--];
			u  = (unsigned char) src[i--] - 128;
			YUV2RGB (y1, u, v, r, g, b);
			dest[j--] = b;
			dest[j--] = g;
			dest[j--] = r;
			YUV2RGB (y0, u, v, r, g, b);
			dest[j--] = b;
			dest[j--] = g;
			dest[j--] = r;
		}
		break;
	default:
		break;
	} 
}
//-----------------------------------------------------------------------------
void XRawImage::YUV411_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height)
{
	int i = (width*height) + ( (width*height) >> 1 )-1;
	int j = (width*height) + ( (width*height) << 1 )-1;
	int y0, y1, y2, y3, u, v;
	int r, g, b;

	while (i >= 0) 
	{
		y3 = (unsigned char) src[i--];
		y2 = (unsigned char) src[i--];
		v  = (unsigned char) src[i--] - 128;
		y1 = (unsigned char) src[i--];
		y0 = (unsigned char) src[i--];
		u  = (unsigned char) src[i--] - 128;
		YUV2RGB (y3, u, v, r, g, b);
		dest[j--] = b;
		dest[j--] = g;
		dest[j--] = r;
		YUV2RGB (y2, u, v, r, g, b);
		dest[j--] = b;
		dest[j--] = g;
		dest[j--] = r;
		YUV2RGB (y1, u, v, r, g, b);
		dest[j--] = b;
		dest[j--] = g;
		dest[j--] = r;
		YUV2RGB (y0, u, v, r, g, b);
		dest[j--] = b;
		dest[j--] = g;
		dest[j--] = r;
	}
}
//-----------------------------------------------------------------------------
void XRawImage::MONO8_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height)
{
	int i = (width*height)-1;
	int j = (width*height) + ( (width*height) << 1 )-1;
	int y;

	while (i >= 0) 
	{
		y = (unsigned char) src[i--];
		dest[j--] = y;
		dest[j--] = y;
		dest[j--] = y;
	}
}
//-----------------------------------------------------------------------------
void XRawImage::MONO16_to_RGB8(unsigned char * src, unsigned char * dest, int width, int height, int bits)
{
	int i = ((width*height) << 1)-1;
	int j = (width*height) + ( (width*height) << 1 )-1;
	int y;

	while (i > 0) 
	{
		y = src[i--];
		y = (y + (src[i--]<<8))>>(bits-8);
		dest[j--] = y;
		dest[j--] = y;
		dest[j--] = y;
	}
}
//-----------------------------------------------------------------------------
//Specific for AVT Pike 12 bits packed...
/*
Notes: 2 pixels are coded inside 3 bytes
First bit: high bits
Second bit: the two low bits
Third bit: high bits
*/
void XRawImage::RGB12_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels)
{
	//On garde les bits de poid faible moche mais temps-réel!!!
	unsigned char * temp_image = (unsigned char *) dest;
	unsigned char * temp_buf = (unsigned char * ) src;
	for (int index =0; index < 3*NumPixels; index +=2 )
	{
		//RGB 1
		* temp_image= *temp_buf;
		temp_image++;

		//RGB2
		* temp_image= *(temp_buf+2);
		temp_image++;

		temp_buf+=3;	
	}

}
//-----------------------------------------------------------------------------
void XRawImage::MONO12_to_RGB8(unsigned char  *src, unsigned char *dest, int NumPixels)
{
	//On garde les bits de poid faible moche mais temps-réel!!!
	unsigned char * temp_image = (unsigned char *) dest;
	unsigned char * temp_buf = (unsigned char * ) src;
	for (int index =0; index < NumPixels; index +=2 )
	{
		//RGB 1
		* temp_image= *temp_buf;
		temp_image++;
		* temp_image= *temp_buf;
		temp_image++;
		* temp_image= *temp_buf;
		temp_image++;

		//RGB2
		* temp_image= *(temp_buf+2);
		temp_image++;
		* temp_image= *(temp_buf+2);
		temp_image++;
		* temp_image= *(temp_buf+2);
		temp_image++;

		temp_buf+=3;	
	}
}

//Presentation avec deux méthodes
void XRawImage::MONO12_to_MONO8(unsigned char * src, unsigned char * dest, int width, int height)
{
	/*
	typedef struct MATIS_12BIT_2PACKED
	{
		unsigned char  m_nVal8_1       ;   //!< High byte of the first Pixel
		unsigned char  m_nVal8_1Low : 4;   //!< Low nibble of the first pixel
		unsigned char  m_nVal8_2Low : 4;   //!< Low nibble of the second pixel
		unsigned char  m_nVal8_2       ;   //!< High byte of the second pixel
	} MATIS_12BIT_2PACKED;
	*/
	unsigned char *	temp_image_in = src;
	unsigned char *	temp_image_8 = dest;
	for(int i = 0;i< width * height;i+=2,temp_image_in+=3,temp_image_8+=2)
	{
		unsigned char char1=*temp_image_in;    //AB
                //unsigned char char2=*(temp_image_in+1);//DC
		unsigned char char3=*(temp_image_in+2);//EF

		* temp_image_8 = char1;
		* (temp_image_8 + 1) = char3;
		
		/*
		MATIS_12BIT_2PACKED tt= *(MATIS_12BIT_2PACKED *)temp_image_in;

		* temp_image = tt.m_nVal8_1 | (tt.m_nVal8_1Low << 12); //D0AB
		* (temp_image + 1) = tt.m_nVal8_2 | (tt.m_nVal8_2Low << 12); //C0EF
		*/
	}
}

//conversion de l'image 12 bits en 8 bits dans un buffer RGB (destiné au Viewer)
void XRawImage::RAW12_to_ViewerRGB8(unsigned char * src, unsigned char * dest, int width, int height)
{
	unsigned char *	in = src;
	unsigned char * out = dest;

	for(int i = 0; i< width * height; i+=2,in+=3,out+=6)
	{
		
		unsigned char char1=*in;    //AB
		unsigned char char2=*(in+1);//DC 
                //unsigned char char3=*(in+2);//EF

		//1 er pixel
		unsigned short val16 = ( char1 << 4 | ((char2 >> 4) & 0x0F)) & 0x0FFF;
		unsigned char val8 = val16 / 16; 
		*out = val8; *(out + 1) = val8; *(out + 2) = val8;
		if( val16 > 4090)
		{
			*out = 255; *(out + 1) = 0; *(out + 2) = 0;
		}

		//2 eme pixel
		val16 =( char1 << 4 | ( char2       & 0x0F)) & 0x0FFF;
		val8 =  val16 / 16; 
		*(out + 3) = val8; *(out + 4) = val8; *(out + 5) = val8;
		if( val16 > 4090)
		{
			*(out + 3) = 255; *(out + 4) = 0; *(out + 5) = 0;
		}
	}
}
//-----------------------------------------------------------------------------
void XRawImage::RAW12_to_RAW16(unsigned char * src, unsigned char * dest, int width, int height)
{
	unsigned char *	in = src;
	unsigned short * out = (unsigned short *)dest;
	for(int i = 0; i< width * height; i+=2,in+=3,out+=2)
	{
		
		unsigned char char1=*in;    //AB
		unsigned char char2=*(in+1);//DC 
		unsigned char char3=*(in+2);//EF

		* out       = ( char1 << 4 | ((char2 >> 4) & 0x0F)) & 0x0FFF;
		* (out + 1) = ( char3 << 4 | ( char2       & 0x0F)) & 0x0FFF;
	}
}
//-----------------------------------------------------------------------------
void XRawImage::RAW16_to_RAW8(unsigned short * src, unsigned char * dest, unsigned long  nbPix)
{
	unsigned short * in = src;
	unsigned char  * out = dest;
	srand(10);
	double random;
	for(int i = 0; i< nbPix; i++)
	{
		random = (double)rand()/(double)RAND_MAX;
		double valFloat = (double)*in / 257.;
		byte valByte = (byte) valFloat;
		double prob = valFloat - valByte;
		if (random<prob)
			*out = (byte) XMax(0., XMin( valFloat + 1, 255.));
		else
			*out = (byte) XMax(0., XMin( valFloat , 255.));
		in++;
		out++;
	}
}
//-----------------------------------------------------------------------------
bool XRawImage::ConvertRAW12_to_RAW16()
{
	if(Bps !=12)
		return XErrorError(m_error,__FUNCTION__," L'image n'est pas en 12 bits ",m_strFileName.c_str());
	if(NbChannel !=1)
		return XErrorError(m_error,__FUNCTION__," L'image n'est pas mono canal ",m_strFileName.c_str());
	
	unsigned short * newPix = new unsigned short[W*H] ;
	RAW12_to_RAW16(Pix,(unsigned char *)newPix,W,H);
	delete Pix;
	Pix = (unsigned char *)newPix;
	Bps = 16;
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::ConvertRGB8_to_BMP()
{
	if((Bps !=8)||(NbChannel !=3))
		return XErrorError(m_error,__FUNCTION__," L'image doit être en 3 canaux sur 8 bits ",m_strFileName.c_str());
	
	unsigned char* stock = new unsigned char[LineW()];
	unsigned char* source = Pix;
	unsigned char* dest  =  Pix + (H-1)*LineW() ;
	size_t linesize = LineSize();
	for(uint32 ligne =0; ligne< H/2; ligne++)
	{
		memcpy(stock,dest,linesize);
		memcpy(dest,source,linesize);
		memcpy(source,stock,linesize);
		source += LineW() ;
		dest  -= LineW() ;
	}
	delete stock;

	unsigned char* p = Pix;
	unsigned char stoc;
	for(uint32 i =0; i< H*W; i++)
	{
		memcpy(&stoc,p+2,1);
		memcpy(p+2,p,1);
		memcpy(p,&stoc,1);
		p+=3;
	}
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::Convert16_to_8()
{
	if(Bps !=16)
		return XErrorError(m_error,__FUNCTION__," L'image doit être en 16 bits ",m_strFileName.c_str());

	uint32 nbpix = W*H*NbChannel;
	unsigned char * newPix = new unsigned char[nbpix] ;

	RAW16_to_RAW8((unsigned short*)Pix,newPix,nbpix);

	delete Pix;
	Pix = newPix;
	Bps = 8;

	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::ConvertRGB8_to_Mono8()
{	
	if(NbChannel !=3)
		return XErrorError(m_error,__FUNCTION__," L'image doit avoir 3 canaux ",m_strFileName.c_str());
	if(Bps !=8)
		return XErrorError(m_error,__FUNCTION__," L'image doit être en 8 bits ",m_strFileName.c_str());

	unsigned char * newPix = new unsigned char[W*H] ;
	unsigned char * in = Pix;
	unsigned char * out = newPix;
	for(int i = 0; i< W*H; i+=1,in+=3,out+=1)
	{
		double mono = (*in+*(in+1)+*(in+2))/3.;
		* out  = (unsigned char)mono;
	}
	delete Pix;
	Pix =  newPix;
	NbChannel =1;
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::ConvertRGB_to_Mono()
{	
	if(NbChannel !=3)
		return XErrorError(m_error,__FUNCTION__," L'image doit avoir 3 canaux ",m_strFileName.c_str());
	if(Bps ==8)
		return ConvertRGB8_to_Mono8();

	unsigned short * newPix = new unsigned short[W*H] ;

	unsigned short * in = (unsigned short *)Pix;
	unsigned short * out = newPix;
	for(int i = 0; i< W*H; i+=1,in+=3,out+=1)
	{
		
		unsigned short r=*in;    //AB
		unsigned short g=*(in+1);//DC 
		unsigned short b=*(in+2);//EF

		double mono = (r+g+b)/3.;
		* out  = (unsigned short)mono;
	}
	delete Pix;
	Pix = (unsigned char *) newPix;

	NbChannel =1;
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::RGB2BGR()
{
	if(NbChannel !=3)
		return XErrorError(m_error,__FUNCTION__," L'image doit avoir 3 canaux ",m_strFileName.c_str());

	if(Bps==8)
	{
		XImage<unsigned char> originale(Width(), Height(),NChannel(),false);
		originale.Pixels(Pixels());
		originale.RGB2BGR();
		originale.Disconnect();
		return true;
	}
	if(Bps==16)
	{
		XImage<unsigned short> originale(Width(), Height(),NChannel(),false);
		originale.Pixels((unsigned short*)Pixels());
		originale.RGB2BGR();
		originale.Disconnect();
		return true;
	}
	return false;
}
//buffer circulaire
//-----------------------------------------------------------------------------
bool XRawImage::TranslateBufferRing(int nbPixeldecal)
{
//	int nbPix = nbPixeldecal%W;

	if(nbPixeldecal < 0)
		return TranslateBufferRingLeft(-nbPixeldecal);
	else
		return TranslateBufferRingRight(nbPixeldecal);
}
//-----------------------------------------------------------------------------
bool XRawImage::IncreaseBufferDown(uint16 new_Height)
{	
	if(new_Height <= Height())
		return XErrorError(m_error,__FUNCTION__," La nouvelle hauteur doit être supérieure à la hauteur courante",m_strFileName.c_str());

	size_t initSize = RawSize();
	unsigned char* temp = new unsigned char[initSize];
	if(temp == NULL)
		return XErrorError(m_error,__FUNCTION__," Erreur d'allocation temporaire");

	memcpy(temp,Pix,initSize);
	if(!Allocate(W, new_Height, Bps, NbChannel))
		return false;
	memcpy(Pix,temp,initSize);
	delete temp;
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::TranslateBufferRingLeft(uint16 nbPixeldecal)
{	

	uint32 decalSize = nbPixeldecal* NbChannel * Bps/8;
	uint32 pixelSize = PixelSize();
	unsigned char* temp = new unsigned char[decalSize];
	for(uint32 i=0; i< H; i++)
	{
		unsigned char* pos = Pix + i*LineSize();
		memcpy(temp, pos, decalSize);
		
		for(uint32 j=0 ; j < W -nbPixeldecal; j++)
		{
			memmove(pos, pos+decalSize, pixelSize);
			pos += pixelSize;
		}
		memcpy(pos, temp, decalSize);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::TranslateBufferRingRight(uint16 nbPixeldecal)
{
	uint32 decalSize = nbPixeldecal* NbChannel * Bps/8;
	uint32 pixelSize = PixelSize();
	unsigned char* temp = new unsigned char[decalSize];

	for(uint32 i=1; i< H+1; i++)
	{
		unsigned char* pos = Pix + i*LineSize() - decalSize;
		memcpy(temp, pos, decalSize);

		for(uint32 j=0 ; j < W - nbPixeldecal; j++)
		{
			memmove(pos+decalSize, pos, pixelSize);
			pos -= pixelSize;
		}
		memcpy(pos+PixelSize(), temp, decalSize);
	}
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::Swap()
{
	if(Bps !=16)
		return XErrorError(m_error,__FUNCTION__," L'image n'est pas en 16 bits ",m_strFileName.c_str());
	if(NbPixels() == 0)
		return XErrorError(m_error,__FUNCTION__," L'image n'a pas de taille ",m_strFileName.c_str());
	if(Pix == NULL)
		return XErrorError(m_error,__FUNCTION__," L'image n'est pas allouée ",m_strFileName.c_str());
	Swap16((unsigned short*)Pix,NbPixels());
	return true;
}
//-----------------------------------------------------------------------------
bool XRawImage::Fill0Value()
{
	if(Pix == NULL)
		return XErrorError(m_error,__FUNCTION__," L'image n'est pas allouée ",m_strFileName.c_str());
	memset(Pix,0,RawSize());
	return true;

}

