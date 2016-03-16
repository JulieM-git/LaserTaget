//-----------------------------------------------------------------------------
//								XErrorXml.h
//								===========
//
// Auteur : F.Becirspahic - Projet Camera Numerique
//
// 22/08/00
//-----------------------------------------------------------------------------

#ifndef _XERRORXML
#define _XERRORXML

#include "XError.h"
#include <fstream>

class XErrorXml : public XError {
protected:
	std::ostream*	m_log;		// Fichier LOG
	std::string		m_tagEnd;	// Champ XML en cours
	bool m_bFluxOnOutput;
	std::string m_strLogFileName;//!! nouveau !! nom du fichier log
public:
	XErrorXml(std::ostream* log = NULL, std::string tagEnd = "</log>")
	{
		m_log = log;
		m_tagEnd = tagEnd;
		m_bFluxOnOutput = false;
	}
	virtual ~XErrorXml() {;}

	std::string LogFileName(){return m_strLogFileName;}
	void LogFileName(std::string name){m_strLogFileName =  name;}

	virtual void Error(const char* origine, const char* mes, Type t = eNull);	// Message d'erreur
	virtual void Info(const char* origine, const char* mes, Type t = eNull);		// Message d'information
	virtual void Alert(const char* origine, const char* mes, Type t = eNull); 	// Message d'alerte

	virtual void Commentaire(const char* origine, const char* mes, Type t = eNull);
	virtual void Tag(const char* mes, const char* tag);

	virtual void WriteToOutput(std::ostream* out);					// Sortie d'un flux

	virtual std::ostream* Output() { return m_log;}
	virtual void Output(std::ostream* log) {m_log = log;}

	virtual void BeginOutput();
	virtual void EndOutput();

    // OTO, pour que linux ne rale pas trop, une ligne et une variable de plus ...
	virtual void StartTag(const char* s)
	{
	    std::string s2(s);
	    StartTag( s2 );
    }
	virtual void StartTag(std::string& s);
	virtual void EndTag();

	//--------------------------------------------------------------------------------------------------------
	//nouvelles methodes sans le type enum mais avec le nom de la données concernée par l'erreur
	//pour un message d'erreur on aurait les 3 infos suivantes :
	//	origine =  nom de classe+methode ayant detecté l'errur
	//	mes = message d'erreur
	//	data = nom de la variable concernée par l'erreur
	virtual void Error(const char* origine, const char* mes,  const char* data);
	virtual void Alert(const char* origine, const char* mes,  const char* data);
	virtual void Info(const char* origine, const char* mes, const char* data);
	//--------------------------------------------------------------------------------------------------------
};

#endif //_XERRORXML
