/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MFSampler.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cm.h>
#include <MF.h>

void MFSamplerFree (MFSampler_p sampler) {
	int objID;

	if (sampler->SampleIDs != (int *) NULL) free (sampler->SampleIDs);
	free (sampler);	
}

MFSampler_p MFSamplerRead (FILE *inFile) {
	int objNum, i;
	MFSampler_p sampler;

	if ((sampler = (MFSampler_p) calloc (1,sizeof (MFSampler_t))) == (MFSampler_p) NULL) return ((MFSampler_p) NULL);
	sampler->SampleIDs = (int *) NULL;

	if (fread (sampler, sizeof (MFSampler_t) - sizeof (int *), 1, inFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d",__FILE__,__LINE__);
		MFSamplerFree (sampler);
		return ((MFSampler_p) NULL);
	}
	if (sampler->Swap != 1) {
        MFSwapHalfWord (&(sampler->Swap));
		MFSwapHalfWord (&(sampler->Type));
        MFSwapWord (&(sampler->SampleNum));
		MFSwapLongWord (&(sampler->ObjNum));
	}
	if ((sampler->SampleIDs = (int *) calloc (sampler->ObjNum, sizeof (int))) == (int *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
		MFSamplerFree (sampler);
		return ((MFSampler_p) NULL);
	}
	
	if (fread (sampler->SampleIDs,sampler->ObjNum * sizeof (int), 1, inFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Reading Error in: %s:%d",__FILE__,__LINE__);
		MFSamplerFree (sampler);
		return ((MFSampler_p) NULL);
	}
	return (sampler);
}

int MFSamplerWrite (MFSampler_p sampler, FILE *outFile) {
	int objID;

	sampler->Swap = 1;
	if (fwrite (sampler,sizeof (MFSampler_t) - sizeof (int *),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Writing Error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}

	if (fwrite (sampler->SampleIDs,sampler->ObjNum * sizeof (int),1,outFile) != 1) {
		CMmsgPrint (CMmsgSysError,"File Writng Error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	return (CMsucceeded);
}
