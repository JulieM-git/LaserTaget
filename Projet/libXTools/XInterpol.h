//-----------------------------------------------------------------------------
//								XInterpol.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 14/08/00
//
// Definition des interpolateurs standards : au plus proche voisin, lineaire,
// cubique et en sin x / x
//-----------------------------------------------------------------------------

#ifndef _XINTERPOL_H
#define _XINTERPOL_H

#include "XBase.h"
#include <limits>

//-----------------------------------------------------------------------------
// XInterpol : interpolation au plus proche voisin
//-----------------------------------------------------------------------------
template<class T> class XInterpol {
protected:
	double  m_dEpsilon;	// Plus petite valeur non nulle
	uint16	m_nWin;		// Taille de la fenetre
	double*	m_X;			// Tableau des valeurs en X
	double* m_Y;			// Tableau des valeurs en Y
	uint32	m_nOffX;	// Offset en X

	void Init(uint16 win, double e);
	virtual double ComputeDouble(double* value, double x, double dx = 1.0);

public:

	XInterpol<T>(double e = 1e-6) { Init(1, e);}
	virtual ~XInterpol() {delete[] m_X; delete[] m_Y;}

	inline uint16 Win() const { return m_nWin;}
	double Compute(T* value, double x, double dx = 1.0);
	double BiCompute(T* value, double x, double y,
										uint32 offset = 0, double dx = 1.0, double dy = 1.0);
	void OffsetX(uint32 off = 0) { m_nOffX = off + 1;}
};

//-----------------------------------------------------------------------------
// Constructeur
//-----------------------------------------------------------------------------
template<class T> void XInterpol<T>::Init(uint16 win, double e)
{
	m_dEpsilon = e;
	m_nWin = win;
	m_X = new double[2 * m_nWin];
	m_Y = new double[2 * m_nWin];
	m_nOffX = 1;
}

//-----------------------------------------------------------------------------
// XInterLin : interpolation lineaire
//-----------------------------------------------------------------------------
template<class T> class XInterLin : public XInterpol<T> {
public :
	XInterLin<T>(double e = 1e-6) { XInterpol<T>::Init(1, e);}
protected:
	virtual double ComputeDouble(double* value, double x, double dx = 1.0);
};

//-----------------------------------------------------------------------------
// XInterCub : interpolation cubique
//-----------------------------------------------------------------------------
template<class T> class XInterCub : public XInterpol<T> {
public :
	XInterCub<T>(double e = 1e-6) { XInterpol<T>::Init(2, e);}
protected:
	virtual double ComputeDouble(double* value, double x, double dx = 1.0);
};

//-----------------------------------------------------------------------------
// XInterSin : interpolation en sin x / x
//-----------------------------------------------------------------------------
template<class T> class XInterSin : public XInterpol<T> {
public :
	XInterSin<T>(uint16 win = 3,double e = 1e-6) { XInterpol<T>::Init(win, e); TabSinXX();}
	virtual ~XInterSin() { if (m_dTab != 0) delete[] m_dTab;}
protected :
	double *m_dTab;		// Tabulation (pour l'interpolatione en sin x / x)
	void TabSinXX();	// Tabulation de la fonction sin / x
	virtual double ComputeDouble(double* value, double x, double dx = 1.0);
};

//-----------------------------------------------------------------------------
// Calcul sur un tableau de reels
//-----------------------------------------------------------------------------
template<class T> double XInterpol<T>::ComputeDouble(double* value, double x, double dx)
{
	if (x <= dx * 0.5)
		return value[0];
	else
		return value[1];
}

//-----------------------------------------------------------------------------
template<class T> double XInterLin<T>::ComputeDouble(double* value, double x, double dx)
{
	double a;
	if (fabs(dx) < XInterpol<T>::m_dEpsilon)
		return value[0];
	a = (value[1] - value[0]) / dx;
	return (a*x + value[0]);
}

//-----------------------------------------------------------------------------
#ifdef max
#undef max
#endif //max

#ifdef min
#undef min
#endif //min
template<class T> double XInterCub<T>::ComputeDouble(double* value, double x, double dx)
{
	double a, b, c, d = value[1];
	double dz1 = value[0] - d, dz2 = value[2] - d, dz3 = value[3] - d;

	if (fabs(dx) < XInterpol<T>::m_dEpsilon)
		return value[1];

	a = (-1. * dz1 - 3. * dz2 + 1. * dz3) / (6. * dx * dx * dx);
	b = ( 3. * dz1 + 3. * dz2) / (6. * dx * dx);
	c = (-2. * dz1 + 6. * dz2 - 1. * dz3) / (6. * dx);
//	double res = (a*x*x*x + b*x*x + c*x + d);
	double res = ((a*x + b)*x + c)*x + d;

	if (res > std::numeric_limits<T>::max())
		return std::numeric_limits<T>::max();
	if(res < std::numeric_limits<T>::min())
		return std::numeric_limits<T>::min();
	return res;
}

//-----------------------------------------------------------------------------
template<class T> double XInterSin<T>::ComputeDouble(double* value, double x, double dx)
{
	int k, n;
	double out, r;

	out = 0.0;
	r = (x + (double)XInterpol<T>::m_nWin)*100.0;
	n = (int)r;
	for(k = 1 - XInterpol<T>::m_nWin; k<= XInterpol<T>::m_nWin;k++){
		n -=100;
		out += m_dTab[(int)abs(n)]*value[k+XInterpol<T>::m_nWin-1];
	}
	return out;
}

//-----------------------------------------------------------------------------
// Creation d'un tableau de valeurs pre-calculees pour l'interpolation en sinx/x
//-----------------------------------------------------------------------------
template<class T> void XInterSin<T>::TabSinXX()
{
	double xx, Pi100, tmp[50];

	Pi100 = 0.03141592654;

	// Allocation de la memoire pour le tableau
	m_dTab = new double[XInterpol<T>::m_nWin*100 + 1];
	if (m_dTab == 0)
		return;

	uint16 L0 = 100 * XInterpol<T>::m_nWin, i, j;
	for (i = 1; i <= L0; i++) {
		xx = Pi100 * (double)i;
		m_dTab[i] = sin(xx) / xx;
	}

	for (j = 1; j <= XInterpol<T>::m_nWin; j++)
		m_dTab[100*j] = 0.0;

	m_dTab[0] = 1.0;

	// Amortissement de la troncature
	for (i = 50; i <= 99; i++) {
		xx = m_dTab[i];
		L0 = 100;
		for (j = 1; j <= XInterpol<T>::m_nWin - 1; j++) {
			xx = xx + m_dTab[L0 - i] + m_dTab[L0 + i];
			L0+=100;
		}
		xx += m_dTab[L0-i];
		tmp[i-50] = (1.0 - xx)*0.5;
	} /* endfor i */

	// Repartition de l'erreur de troncature
	L0 = 100 * (XInterpol<T>::m_nWin - 1);
	for (i = 50; i <= 99; i++)
		m_dTab[L0 + i] += tmp[i-50];
	for (i = 1; i <= 49; i++)
		m_dTab[L0 + i] += tmp[50-i];
}

//-----------------------------------------------------------------------------
// Calcul sur un tableau de type quelconque
//-----------------------------------------------------------------------------
template<class T> double XInterpol<T>::Compute(T* value, double x, double dx)
{
	for (uint16 i = 0; i < 2 * m_nWin; i++)
		m_X[i] = (double)value[i*m_nOffX];
	return ComputeDouble(m_X, x, dx);
}

template<class T> double XInterpol<T>::BiCompute(T* value, double x, double y,
											  uint32 offset, double dx, double dy)
{
	for (uint16 i = 0; i < 2 * m_nWin; i++)
		m_Y[i] = Compute(&value[i*(2*m_nWin + offset)], x, dx);
	return ComputeDouble(m_Y, y, dy);
}

#endif //_XINTERPOL_H
