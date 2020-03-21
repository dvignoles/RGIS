/******************************************************************************

GHAAS Variable Database library V2.1
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

vdb.h

bfekete@gc.cuny.edu

*******************************************************************************/

#ifndef _VDB_H
#define _VDB_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct VDBmasterTable_s {
    const char *Group;
    const char *CFname;
    const char *GHAASname;
    const char *LongName;
    const char *DataType;
    const char *Shadeset;
    const char *Aggregation;
} VDBmasterTable_t;

const char *VDBrgName      (const char*);
const char *VDBlongName    (const char*);
const char *VDBcfName      (const char*);
const char *VDBdataType    (const char*);
const char *VDBshadset     (const char*);
const char *VDBaggregation (const char*);

#if defined(__cplusplus)
}
#endif

#endif // _VDB_H
