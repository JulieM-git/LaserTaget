#ifndef _XFILETXTREADER_H
#define _XFILETXTREADER_H

#include "XError.h"
#include <fstream>

typedef bool (*F_ProcessLine)(char* line);
class  XFileTxtReader
{
public:
	XError* m_error;
	std::string m_strFilename;
	uint32 NbLines;
	uint32 NbComments;
	char* ligne;
protected:
	bool ReadLine(std::istream* in);
public:
	 XFileTxtReader(const char* filename, XError* error) 
	 {
		 m_strFilename = filename;
		 m_error = error;
		 NbLines = 0;
		 NbComments = 0;
		 ligne = new char[1024];
	 }
	 bool Read(F_ProcessLine ProcessLine, F_ProcessLine ProcessComment = NULL);
};

#endif //_XFILETXTREADER_H
