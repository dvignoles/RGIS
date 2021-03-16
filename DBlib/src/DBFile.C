/******************************************************************************

GHAAS Database library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

DBFile.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <sys/stat.h>
#include <DB.H>

DBInt DBFileSize(const char *fileName) {
    struct stat fileStat;

    if (stat(fileName, &fileStat) == DBFault) return (DBFault);
    return (fileStat.st_size);
}
