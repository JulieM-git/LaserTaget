#pragma once
//-----------------------------------------------------------------------------
//XErrorTxt.h
//JPP
#include "XError.h"
#include <fstream>

class XErrorTxt: public XError 
{
protected:
	std::ostream*	m_log;		// flux de sortie
	std::string m_strLogFileName;// nom du fichier log
	bool m_bLogCommentaire;

public:
	XErrorTxt(std::ostream* log);
	~XErrorTxt(void);

	void LogCommentaire(bool flag) { m_bLogCommentaire = flag;}

	std::string LogFileName(){return m_strLogFileName;}
	void LogFileName(std::string name){m_strLogFileName =  name;}
	virtual std::ostream* Output() { return m_log;}
	virtual void Output(std::ostream* out) {m_log = out;}

	virtual void Error(const char* origine, const char* mes, Type t = eNull);	// Message d'erreur
	virtual void Info(const char* origine, const char* mes, Type t = eNull);		// Message d'information
	virtual void Alert(const char* origine, const char* mes, Type t = eNull); 	// Message d'alerte
	//--------------------------------------------------------------------------------------------------------
	//nouvelles methodes sans le type enum mais avec le nom de la données concernée par l'erreur
	//pour un message d'erreur on aurait les 3 infos suivantes :
	//	origine =  nom de classe+methode ayant detecté l'errur
	//	mes = message d'erreur
	//	data = nom de la variable concernée par l'erreur
	virtual void Error(const char* origine, const char* mes,  const char* data);
	virtual void Alert(const char* origine, const char* mes,  const char* data);
	virtual void Info(const char* origine, const char* mes, const char* data);
	virtual void Commentaire(const char* origine, const char* mes);
};

