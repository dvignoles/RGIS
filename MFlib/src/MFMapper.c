/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

MFMapper.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cm.h>
#include <MF.h>

void MFMapperFree (MFMapper_p mapper) {
	int objID;

	if (mapper->SampleIDs != (int *) NULL) free (mapper->SampleIDs);
	free (mapper);	
}

MFMapper_p MFMapperRead (FILE *inFile) {
	int objNum, i;
	MFMapper_p mapper;

	if ((mapper = (MFMapper_p) calloc (1,sizeof (MFMapper_t))) == (MFMapper_p) NULL) return ((MFMapper_p) NULL);
	mapper->SampleIDs = (int *) NULL;

	if (fread (mapper, sizeof (MFMapper_t) - sizeof (int *), 1, inFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d",__FILE__,__LINE__);
		MFMapperFree (mapper);
		return ((MFMapper_p) NULL);
	}
	if (mapper->Swap != 1) {
        MFSwapHalfWord (&(mapper->Swap));
		MFSwapHalfWord (&(mapper->Type));
        MFSwapWord (&(mapper->SampleNum));
		MFSwapLongWord (&(mapper->ObjNum));
	}
	if ((mapper->SampleIDs = (int *) calloc (mapper->ObjNum, sizeof (int))) == (int *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
		MFMapperFree (mapper);
		return ((MFMapper_p) NULL);
	}
	
	if (fread (mapper->SampleIDs,mapper->ObjNum * sizeof (int), 1, inFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d",__FILE__,__LINE__);
		MFMapperFree (mapper);
		return ((MFMapper_p) NULL);
	}
	return (mapper);
}

int MFMapperWrite (MFMapper_p mapper, FILE *outFile) {
	int objID;

	mapper->Swap = 1;
	if (fwrite (mapper,sizeof (MFMapper_t) - sizeof (int *),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Writing Error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}

	if (fwrite (mapper->SampleIDs,mapper->ObjNum * sizeof (int),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Writng Error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}
