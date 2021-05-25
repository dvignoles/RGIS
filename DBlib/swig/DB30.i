
%module DB30

// Add necessary symbols to generated header
%{
#include "DB.H"
#include "DBif.H"
#include "DBtf.h"
%}

// ignore everything
%ignore "";
%define %unignore %rename("%s") %enddef

// do unignoring 
%unignore DBFileSize(const char *);

// Process symbols in header
%include "DB.H"
%include "DBif.H"
%include "DBtf.h"