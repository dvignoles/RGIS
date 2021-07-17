/******************************************************************************

GHAAS Variable Database library V2.1
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

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

typedef enum { VDBversion2 = 2, VDBversion3 = 3 } VDBversion;

const char *VDBrgName      (VDBversion, const char*);
const char *VDBlongName    (VDBversion, const char*);
const char *VDBcfName      (VDBversion, const char*);
const char *VDBdataType    (VDBversion, const char*);
const char *VDBshadset     (VDBversion, const char*);
const char *VDBaggregation (VDBversion, const char*);

#if defined(__cplusplus)
}
#endif

#endif // _VDB_H
