#ifndef __X_IMAGE_H__
#define __X_IMAGE_H__

#include <iostream>
#include <cmath>
#include <vector>
 #include <cstring>

#include "XBase.h"
#include "XInterpol.h"

class XError;
class XWait;
template<class T> class XImage 
{
protected:
	XError*	m_error;
	XWait* m_wait;

	uint32	W;		// Largeur
	uint32	H;		// Hauteur
	uint32 NChannel;//Num channel

	T*		Pix;	// Tableau des pixels (eventuellement vide)
public:
	XImage(uint32 w, uint32 h, uint32 NbChannel, bool allocate);
	XImage(uint32 w, uint32 h);		
	XImage (int w, int h, const T v) ;//Construction d'une image monocanal dont toutes les valeurs sont mises à v
	XImage(const XImage<T>&);
	virtual ~XImage();

	void Error(XError* error) { m_error = error;}
	XError* Error() { return m_error;}
	bool IsLoaded() { if (Pix == NULL) return false; return true;}

	void Wait(XWait* wait) { m_wait = wait;}
	XWait* Wait() { return m_wait;}

	bool Unload();

	// Operateur de copie
	XImage<T>& operator=(const XImage<T>& M);
	XImage<T>& operator+=(const XImage<T>& M);

	//Operateur d'accès
	T operator () (int x, int y) const;
	T operator () (int x, int y, int c) const;

	T & operator () (int x, int y);
	T & operator () (int x, int y, int c);


	// Geometrie de l'image
	uint32 Width() const { return W;}
	uint32 Height() const { return H;}
	uint32 NbChannel() const{ return NChannel;}
	uint32 RawSize() const{ return LineW() * H * sizeof(T);}
	uint32 LineW() const { return W * NChannel;}

	void CopierCanal (XImage < T > const & src, int nsrc, int ndest) ;
	
	//Pixels accessors
	T* Col(uint32 x, uint32 y = 0) const;
	T* Line(uint32 y) const;

	T* Pixels() const { return Pix;}
	void Pixels(T* pix) { Unload(); Pix = pix;}
	inline void Disconnect() { Pix = NULL;}

	float GetFloatPixBilin(double x, double y, uint16 canal=0);
	T GetPixBilin(double x, double y, uint16 canal=0);

	//Tools
	void Stat(  std::vector<T> & min, 
				std::vector<T> & max, 
				std::vector<double> & mean, 
				std::vector<double> & var,
				uint32 x, uint32 y, uint32 w, uint32 h);

	void Histo( std::vector<T> min, 
				std::vector<T> max, 
				std::vector<uint32*> Histo,
				uint32 x, uint32 y, uint32 w, uint32 h);

	void Add(T lvl, T max, uint32 x, uint32 y, uint32 w, uint32 h);
	void Sub(T lvl, T min, uint32 x, uint32 y, uint32 w, uint32 h);
	void Mult(double lvl, T max, uint32 x, uint32 y, uint32 w, uint32 h);

	bool GetLine(uint32 num, T* line, uint32 x = 0, uint32 w = 0);
	bool GetCol(uint32 num, T* col);
	bool GetArea(uint32 x, uint32 y, uint32 w, uint32 h, T* area);

	bool Cut(uint32 x, uint32 y, uint32 w, uint32 h);

	//Opérations sur les pixels
	bool RGB2BGR();

};

/****************************
	Initialisation
******************************/
template<class T> T XImage<T>::operator () (int x, int y) const 
{
	return  *(Pix + (y *LineW()) + x * NChannel);
}
//-----------------------------------------------------------------------------
template<class T> T XImage<T>::operator () (int x, int y, int c) const
{
	return  *(Pix + (y *LineW()) + x * NChannel + c);
}
//-----------------------------------------------------------------------------
template<class T>  T & XImage<T>::operator () (int x, int y)
{
	return  *(Pix + (y *LineW()) + x * NChannel);
}
//-----------------------------------------------------------------------------
template<class T> T & XImage<T>::operator () (int x, int y, int c)
{
	return  *(Pix + (y *LineW()) + x * NChannel+c);
}

//-----------------------------------------------------------------------------
template<class T> XImage<T>::XImage(uint32 w, uint32 h, uint32 NbChannel,bool allocate)
{
	m_error = NULL;
	m_wait = NULL;
	Pix = NULL;
	W = w; 
	H =h;
	NChannel= NbChannel;
	if(allocate)
	{
		Pix = new T[H * LineW()];
		memset(Pix,0,RawSize());
	}
}
//-----------------------------------------------------------------------------
template<class T> XImage<T>::XImage(uint32 w, uint32 h)
{
	m_error = NULL;
	m_wait = NULL;
	NChannel= 1;
	W = w; 
	H =h;
	Pix = new T[H * LineW()];
	memset(Pix,0,RawSize());
}
//-----------------------------------------------------------------------------
template<class T>  XImage<T>::XImage (int w, int h, const T v)
{
	m_error = NULL;
	m_wait = NULL;
	W = w; 
	H =h;
	NChannel= 1;
	Pix = new T[H * LineW()];
	memset(Pix,0,RawSize());

	for(long i =0; i< (long)(H * LineW());i++)
		Pix[i] = v;
}
//----------------Constructeur de copie--------------------------------
template<class T> XImage<T>::XImage(const XImage<T>& M)
{
	Pix = NULL;
	m_error = M.m_error;
	m_wait = M.m_wait;
	
	W = M.W; 
	H = M.H;
	NChannel = M.NChannel;

	if (M.Pix != NULL) 
	{
		Pix = new T[H * LineW()];
		memcpy(Pix, M.Pix, RawSize());
	}
}
//-----------------------------------------------------------------------------
template<class T> XImage<T>::~XImage()
{
	Unload();
}

//-----------------------------------------------------------------------------
// Décharge l'image de la mémoire
template<class T> bool XImage<T>::Unload()
{
	if (Pix == NULL)
		return true;
	delete[] Pix; 
	Pix = NULL;
	return true;
}
//-----------------------------------------------------------------------------
// Operateur de copie
template<class T> XImage<T>& XImage<T>::operator=(const XImage<T>& M)
{
	if (this == &M)
		return *this;
	Unload();

	W = M.W; H = M.H;
	NChannel =  M.NChannel;

	if (M.Pix != NULL) 
	{
		Pix = new T[LineW() * H];
		memcpy(Pix, M.Pix, H * LineW() * sizeof(T)/8);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Moyenne de deux images
template<class T> XImage<T>& XImage<T>::operator+=(const XImage<T>& M)
{
	if ((W != M.W)||(H != M.H)) 
		return *this;

	T *ptr1 =Pix;
	T *ptr2 =M.Pixels();

	for (unsigned long i = 0; i < H * LineW(); i++)
		ptr1[i] = (T)ceil(((double)ptr1[i] + (double)ptr2[i]) * 0.5);
	
	return *this;
}

/****************************
	Accessors
******************************/
template<class T> T* XImage<T>::Col(uint32 x, uint32 y ) const { return &Pix[x*NChannel+y *LineW() ];}
template<class T> T* XImage<T>::Line(uint32 y) const {return &Pix[y*LineW()];}

/*******************************
	Processing
*******************************/
//-----------------------------------------------------------------------------
// Moyenne et statistiques
template<class T> void XImage<T>::Stat(  std::vector<T> & min, 
										 std::vector<T> & max, 
										 std::vector<double> & mean, 
										 std::vector<double> & var,
										 uint32 x, uint32 y, uint32 w, uint32 h)
{
	if (w == 0)
		w = W - x;
	if (h == 0)
		h = H - y;

	for(uint32 i =0; i< NChannel;i++)
	{
		min.push_back(Pix[i]);
		max.push_back(Pix[i]);
		mean.push_back(0.0);
		var.push_back(0.0);
	}

	for(uint32 k =0; k< NChannel;k++)
	{
		//Put the origin of the pointer at the beginning of the interess zone
		T* ptr = Pix + x * k + y * LineW();

		for (uint32 i = 0; i < h - y; i++)
		{
			for (uint32 j = 0; j < w - x; j++,ptr += LineW())
			{
				min[k] = XMin(min[k], *ptr);
				max[k] = XMax(max[k], *ptr);
				var[k] += (*ptr * *ptr);
				mean[k] += *ptr;
			}
		}
		unsigned zone_image_size= (w-x) * (h-y);
		mean[k]/=zone_image_size;

		var[k] /= zone_image_size;
		var[k] -= (mean[k]* mean[k]);
	}
}

//-----------------------------------------------------------------------------
// Caclul d'histogramme
template<class T> void XImage<T>::Histo(std::vector<T> min, 
										std::vector<T> max, 
										std::vector<uint32*> Histo,
										uint32 x, uint32 y, uint32 w, uint32 h)
{
	if (w == 0)
		w = W - x;
	if (h == 0)
		h = H - y;

	for(uint32 k=0; k<NChannel;k++)
	{
		uint32* TempHisto = Histo[k];
		for (uint32 i = 0; i <= max[k]; i++)
			TempHisto[i] = 0;
	}

	for(uint32 k=0; k<NChannel;k++)
	{
		T* ptr = Pix + x * k + y * LineW();
		uint32* TempHisto = Histo[k];

		for (uint32 i = 0; i < h - y; i++)
		{
			for (uint32 j = 0; j < w - x; j++,ptr += LineW())
			{
				if ((*ptr >= min[k])&&(*ptr <= max[k]))
					TempHisto[(int)*ptr]++;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Addition, soustraction, multiplication
template<class T> void XImage<T>::Add(T lvl, T max, uint32 x, uint32 y, uint32 w, uint32 h)
{
	if (w == 0)
		w = W - x;
	if (h == 0)
		h = H - y;

	T* ptr = Pix + x * NChannel + y * LineW();
	for(unsigned long i =0; i< (h - y) * (w - x) * NChannel;i++)
	{
		ptr[i] += lvl;
		if (ptr[i] > max)	// Valeur limite permise
			ptr[i] = max;
	}
}
//-----------------------------------------------------------------------------
template<class T> void XImage<T>::Sub(T lvl, T min, uint32 x, uint32 y, uint32 w, uint32 h)
{
	if (w == 0)
		w = W - x;
	if (h == 0)
		h = H - y;

	T* ptr = Pix + x * NChannel + y * LineW();
	for(unsigned long i =0; i< (h - y) * (w - x) * NChannel;i++)
		(ptr[i] < min + lvl)?ptr[i] = min:ptr[i] -= lvl;

}
//-----------------------------------------------------------------------------
template<class T> void XImage<T>::Mult(double lvl, T max, uint32 x, uint32 y, uint32 w, uint32 h)
{
	if (w == 0)
		w = W - x;
	if (h == 0)
		h = H - y;

	T* ptr = Pix + x * NChannel + y * LineW();

	T maxL = (T) ceil(max / lvl);
	for(unsigned long i =0; i< (h - y) * (w - x) * NChannel;i++)
		(ptr[i] > maxL)?ptr[i] = max:ptr[i] *= (T)ceil(lvl);
}
//-----------------------------------------------------------------------------
// Redecoupage de l'image
template<class T> bool XImage<T>::Cut(uint32 x, uint32 y, uint32 w, uint32 h)
{
	XImage<T> M(w -x, h-y, NChannel);

	T* ptr = Pix + x * NChannel + y * LineW();
	memcpy(M.Pixels(),ptr,(h - y) * (w - x) * NChannel * sizeof(T)/8);
	*this = M;
	return true;
}

//-----------------------------------------------------------------------------
template<class T> bool XImage<T>::GetLine(uint32 num, T* line, uint32 x, uint32 w)
{
	XAssert(Pix != NULL);
	XAssert(num < H);
	XAssert(x < W - 1);
	if (w == 0) 
		w = W - x;
//	XAssert(x + w < W);
	XAssert(x + w <= W);// <- essai JPP
	T* ptr = Col(x, num);
	for (uint32 i = 0; i < w; i++) {
		line[i] = *ptr;
		ptr += NChannel;
	}
	return true;
}
//-----------------------------------------------------------------------------
template<class T> bool XImage<T>::GetCol(uint32 num, T* col)
{
	XAssert(Pix != NULL);
	XAssert(num < W);
	T* ptr = Pix + (num*NChannel);
	for (uint32 i = 0; i < H; i++)
	{
		col[i] = *ptr;
		ptr += LineW();
	}
	return true;
}
//-----------------------------------------------------------------------------
// Lecture d'une zone
template<class T> bool XImage<T>::GetArea(uint32 x, uint32 y, uint32 w, uint32 h, T* area)
{
	XAssert((x+w <= W) && (y+h <= H));	// Le rectangle doit etre contenu dans l'image

	T* ptr = area;
	for (uint32 i = y; i < y + h; i++){
		GetLine(i, ptr, x, w);
		ptr += w;
	}
	return true;
}
//-----------------------------------------------------------------------------
template < class T > 
void XImage<T>::CopierCanal (XImage < T > const & src, int nsrc, int ndest) 
{
	for(uint32 i =0; i< W;i++)
		for(uint32 j=0;j<H;j++)
			(*this)(i,j,ndest)=src(i,j,nsrc);
}
//-----------------------------------------------------------------------------
template<class T> 
T XImage<T>::GetPixBilin(double x, double y, uint16 canal)
{ 
	return (T)GetFloatPixBilin(x,y,canal);
}
//-----------------------------------------------------------------------------
template<class T> 
float XImage<T>::GetFloatPixBilin(double x, double y, uint16 canal)
{
	XAssert(Pix != NULL);	
	x = XMin((double)W-1,XMax(0.,x));
	y = XMin((double)H-1,XMax(0.,y));
	uint16 col1 = floor(x);
	uint16 lig1 = floor(y);
	double dcol = x-col1; 
	double dlig = y-lig1;

	float l1c1 = operator ()(col1,lig1,canal);
	float l1c2 = operator ()(col1+1,lig1,canal);
	float l2c1 = operator ()(col1,lig1+1,canal);
	float l2c2 = operator ()(col1+1,lig1+1,canal);

	float interl1 = l1c1 + dcol * (l1c2-l1c1);
	float interl2 = l2c1 + dcol * (l2c2-l2c1);
	float res = interl1 + dlig * (interl2 - interl1);
	return res ;
}
//Opérations sur les pixels
//-----------------------------------------------------------------------------
template<class T> 
bool XImage<T>::RGB2BGR()
{
	XAssert(Pix != NULL);	
	if(NChannel != 3)
		return false;

	T* ppix = Pix;
	for(uint32 i =0; i< W*H;i++)
	{
		T temp = *ppix;
		*ppix = *(ppix+2);
		ppix = ppix+2;
		*ppix = temp;
	}
	
	return true;
}


#endif

