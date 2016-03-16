#pragma once
#include "XErrorTxt.h"
#include "ConsoleError.h"

class XErrorConsole : public XError
{
public:
	bool m_bShowOrigine;
public:
	XErrorConsole(void) : XError(){m_bShowOrigine = false;}
	~XErrorConsole(void){;}
	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		ConsoleError::Error(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		ConsoleError::Error(origine,mes,data,m_bShowOrigine);
	}

	virtual void Alert(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		ConsoleError::Alert(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Alert(const char* origine, const char* mes,  const char* data) 
	{
		ConsoleError::Alert(origine,mes,data,m_bShowOrigine);
	}

	virtual void Info(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		ConsoleError::Info(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Info(const char* origine, const char* mes,  const char* data) 
	{
		ConsoleError::Info(origine,mes,data,m_bShowOrigine);
	}
	virtual void Commentaire(const char* origine, const char* mes) 
	{
		ConsoleError::Commentaire(origine,mes,m_bShowOrigine);
	}
};

class XErrorConsoleTxt : public XErrorTxt
{
public:
	bool m_bShowOrigine;
public:
	XErrorConsoleTxt(std::ostream* log = NULL) : XErrorTxt(log){m_bShowOrigine = false;}
	~XErrorConsoleTxt(void){;}
	virtual void Error(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorTxt::Error(origine, mes,t);
		ConsoleError::Error(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Error(const char* origine, const char* mes,  const char* data) 
	{
		XErrorTxt::Error(origine, mes,data);
		ConsoleError::Error(origine,mes,data,m_bShowOrigine);
	}

	virtual void Alert(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorTxt::Alert(origine, mes,t);
		ConsoleError::Alert(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Alert(const char* origine, const char* mes,  const char* data) 
	{
		XErrorTxt::Alert(origine, mes,data);
		ConsoleError::Alert(origine,mes,data,m_bShowOrigine);
	}

	virtual void Info(const char* origine, const char* mes, Type t = XError::eNull)
	{ 
		XErrorTxt::Info(origine, mes,t);
		ConsoleError::Info(origine,mes,TypeString(t).c_str(),m_bShowOrigine);
	}

	virtual void Info(const char* origine, const char* mes,  const char* data) 
	{
		XErrorTxt::Info(origine, mes,data);
		ConsoleError::Info(origine,mes,data,m_bShowOrigine);
	}
	virtual void Commentaire(const char* origine, const char* mes) 
	{ //on ne voit pas l'origine pour les commentaires
		XErrorTxt::Commentaire("", mes);
		ConsoleError::Commentaire("",mes,false);
	}
};

