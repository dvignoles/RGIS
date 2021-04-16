%module pyDBlib

// Add necessary symbols to generated header
%{
#define SWIG_FILE_WITH_INIT
#include <DB.H>
%}

// ignore everything
%ignore "";
%define %unignore %rename("%s") %enddef

// do unignoring 
%unignore DBObjData;
%unignore DBImportNetCDF(DBObjData *, const char *);

%include <DB.H>

// %unignore ""; // unignore all




