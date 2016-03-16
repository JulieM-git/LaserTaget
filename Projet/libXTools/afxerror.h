#pragma once
#include "XErrorXml.h"
#include "XErrorConsole.h"
#include "XErrorTxt.h"
//#include <stdio.h>
#include <iostream>

class CAfxErrorXml : public XErrorXml
{
public:
	CAfxErrorXml(std::ostream* log = NULL) : XErrorXml(log){;}
	~CAfxErrorXml(void){;}
	//seuls les messages d'erreur sont surchargés et provoquent un message
	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorXml::Error(origine, mes,t);
		AfxMessageError("ERREUR",origine,mes,TypeString(t).c_str());
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		XErrorXml::Error(origine, mes,data);
		AfxMessageError("ERREUR",origine,mes,data);
	}
	virtual void AfxMessageError(const char * prefixe, const char* origine, const char* mes,  const char* data) 
	{
		std::stringstream OMD;
		OMD <<  prefixe;
		if(strlen(origine)>0)
			OMD <<  "\r\nOrigine : " << origine ;
		if(strlen(mes)>0)
			OMD <<  "\r\nMessage : " << mes ;
		if(strlen(data)>0)
			OMD <<  "\r\nDonnée : " << data ;
		OMD << std::endl;
		AfxMessageBox(OMD.str().c_str(), MB_OK | MB_ICONSTOP);
	}
};

class CAfxErrorTxt : public XErrorTxt
{
public:
	CAfxErrorTxt(std::ostream* log = NULL) : XErrorTxt(log){;}
	~CAfxErrorTxt(void){;}
	//seuls les messages d'erreur sont surchargés et provoquent un message
	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorTxt::Error(origine, mes,t);
		AfxMessageError("ERREUR",origine,mes,TypeString(t).c_str());
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		XErrorTxt::Error(origine, mes,data);
		AfxMessageError("ERREUR",origine,mes,data);
	}
	virtual void AfxMessageError(const char * prefixe, const char* origine, const char* mes,  const char* data) 
	{
		std::stringstream OMD;
		OMD <<  prefixe;
		if(strlen(origine)>0)
			OMD <<  "\r\nOrigine : " << origine ;
		if(strlen(mes)>0)
			OMD <<  "\r\nMessage : " << mes ;
		if(strlen(data)>0)
			OMD <<  "\r\nDonnée : " << data ;
		OMD << std::endl;
		AfxMessageBox(OMD.str().c_str(), MB_OK | MB_ICONSTOP);
	}
};
class CAfxErrorTxtConsole : public XErrorConsoleTxt
{
public:
	CAfxErrorTxtConsole(std::ostream* log = NULL) : XErrorConsoleTxt(log){;}
	~CAfxErrorTxtConsole(void){;}

	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorConsoleTxt::Error(origine, mes,t);
		AfxMessageError("ERREUR",origine,mes,TypeString(t).c_str());
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		XErrorConsoleTxt::Error(origine, mes,data);
		AfxMessageError("ERREUR",origine,mes,data);
	}
	virtual void AfxMessageError(const char * prefixe, const char* origine, const char* mes,  const char* data) 
	{
		std::stringstream OMD;
		OMD <<  prefixe;
		if(strlen(origine)>0)
			OMD <<  "\r\nOrigine : " << origine ;
		if(strlen(mes)>0)
			OMD <<  "\r\nMessage : " << mes ;
		if(strlen(data)>0)
			OMD <<  "\r\nDonnée : " << data ;
		OMD << std::endl;
		AfxMessageBox(OMD.str().c_str(), MB_OK | MB_ICONSTOP);
	}
};
class CAfxError : public XError
{
public:
	CAfxError() : XError(){;}
	~CAfxError(void){;}
	//seuls les messages d'erreur sont surchargés et provoquent un message
	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		AfxMessageError("ERREUR",origine,mes,TypeString(t).c_str());
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		AfxMessageError("ERREUR",origine,mes,data);
	}
	virtual void AfxMessageError(const char * prefixe, const char* origine, const char* mes,  const char* data) 
	{
		std::stringstream OMD;
		OMD <<  prefixe;
		if(strlen(origine)>0)
			OMD <<  "\r\nOrigine : " << origine ;
		if(strlen(mes)>0)
			OMD <<  "\r\nMessage : " << mes ;
		if(strlen(data)>0)
			OMD <<  "\r\nDonnée : " << data ;
		OMD << std::endl;
		AfxMessageBox(OMD.str().c_str(), MB_OK | MB_ICONSTOP);
	}
};
