/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDdsClimatology.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>


static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgUsrError, "%s [options] <datastream1> <datastream2> ... <datastreamN>", CMfileName(arg0));
    CMmsgPrint(CMmsgUsrError, "  -o, --output <datastream filename>");
    CMmsgPrint(CMmsgUsrError, "  -h,--help");
}

enum { DAY = 10, MONTH = 7, YEAR = 4 };
enum { AVG = 1, SUM = 2 };

int main(int argc, char *argv[]) {
    bool inCompress = false, outCompress = false;
    int argPos = 0, argNum = argc, ret = CMfailed, itemType, itemSize, itemNum, i, recordID = 0, recordNum = 0;
    char *outFileName = (char *) NULL;
    FILE *inFile = stdin, *outFile = stdout;
    MFdsHeader_t header, outHeader;
    void    *items = (void *)   NULL;
    char   **dates = (char **)  NULL;
    int  **obsNums = (int **)   NULL;
    float **arrays = (float **) NULL;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest(argv[argPos], "-o", "--output")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing output file!");
                return (CMfailed);
            }
            outFileName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
Help:   if (CMargTest(argv[argPos], "-h", "--help")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) < argPos) break;
            _CMDprintUsage (argv[0]);
            ret = CMsucceeded;
            goto Stop;
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    for (argPos = 1; argPos < argNum; ++argPos) {
        if (strncmp(CMfileExtension(argv[argPos]), "gz", 2) == 0) {
            char pCommand[strlen(argv[argPos]) + 16];
            sprintf(pCommand, "gunzip -c %s", argv[argPos]);
            inFile = popen(pCommand, "r");
            inCompress = true;
        }
        else {
            inFile = fopen(argv[argPos], "r");
            inCompress  = false;
        }
        if (inFile == (FILE *) NULL) {
            CMmsgPrint (CMmsgUsrError, "Skipping file: %s",argv[argPos]);
            goto Next;
        }
        recordID = 0;
        while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
            if ((strlen (header.Date) > 4) && (strcmp(header.Date + 4,"-02-29") != 0)) { // Skipping February 29th
                if (recordNum == 0) {
                    itemNum  = header.ItemNum;
                    itemType = header.Type;
                    itemSize = MFVarItemSize(header.Type);
                    if ((items = (void *) calloc(header.ItemNum, itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                }
                else {
                    if ((itemNum != header.ItemNum) || (itemType != header.Type)) {
                        CMmsgPrint (CMmsgUsrError,"Skipping inconsistent datastream file: %s", argv[argPos]);
                        goto Next;
                    }
                }
                if (recordID >= recordNum) {
                    recordNum = recordID + 1;
                    if ((arrays = (float **) realloc (arrays,recordNum * sizeof(float *))) == (float **) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((arrays[recordID] = (float *) calloc (itemNum,sizeof(float))) == (float *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((dates = (char **) realloc (dates,recordNum * sizeof(char *))) == (char **) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((dates[recordID] = (char *) malloc (MFDateStringLength)) == (char *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((obsNums = (int **) realloc (obsNums,recordNum * sizeof(int *))) == (int **) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((obsNums[recordID] = (int *) calloc (itemNum,sizeof(int))) == (int *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if (strlen (header.Date) == 4) sprintf (dates[recordID],"XXXX"); else sprintf (dates[recordID],"XXXX%s",header.Date + 4); 
                    for (i = 0; i < itemNum; ++i) { arrays[recordID][i] = 0.0; obsNums[recordID][i] = 0; }
                }
                if (fread(((char *) items), itemSize, itemNum, inFile) != itemNum) {
                    CMmsgPrint (CMmsgSysError,"File reading error: %s",argv[argPos]);
                    goto Stop;
                }
                for (i = 0; i < itemNum; i++) {
                    switch (itemType) {
                        case MFByte:
                            if (((char *) items)[i] != header.Missing.Int) {
                                 arrays[recordID][i] += (float) (((char *) items)[i]);
                                obsNums[recordID][i] += 1;
                            }
                            break;
                        case MFShort:
                            if (header.Swap != 1) MFSwapHalfWord(((short *) items) + i);
                            if (((short *) items)[i] != header.Missing.Int) {
                                 arrays[recordID][i] += (float) (((short *) items)[i]);
                                obsNums[recordID][i] += 1;
                            }
                            break;
                        case MFInt:
                            if (header.Swap != 1) MFSwapWord(((int *) items) + i);
                            if (((int *) items)[i] != header.Missing.Int) {
                                 arrays[recordID][i] += (float) (((int *) items)[i]);
                                obsNums[recordID][i] += 1;
                            }
                            break;
                        case MFFloat:
                            if (header.Swap != 1) MFSwapWord(((float *) items) + i);
                            if ((isnan(((float *) items)[i]) == false) && (CMmathEqualValues(((float *) items)[i], header.Missing.Float) == false)) {
                                 arrays[recordID][i] += ((float *) items)[i];
                                obsNums[recordID][i] += 1;
                            }
                            break;
                        case MFDouble:
                            if (header.Swap != 1) MFSwapLongWord(((double *) items) + i);
                            if ((isnan (((double *) items)[i]) == false) && (CMmathEqualValues(((double *) items)[i], header.Missing.Float) == false)) {
                                 arrays[recordID][i] += (float) (((double *) items)[i]);
                                obsNums[recordID][i] += 1;
                            }
                            break;
                    }
                }
                if (feof(inFile) != 0) goto Next;
                recordID++;
            }
            else {
                if (fread(((char *) items), itemSize, itemNum, inFile) != itemNum) {
                    CMmsgPrint (CMmsgSysError,"File reading error: %s",argv[argPos]);
                    goto Stop;
                }
            }
        }
Next:   if (inFile != (FILE *) NULL) { if (inCompress) pclose (inFile); else fclose (inFile); }
        recordID = 0;
    }
    outHeader.Swap = 1;
    outHeader.Type = MFFloat;
    outHeader.ItemNum = itemNum;
    outHeader.Missing.Float = (header.Type == MFFloat) || (header.Type == MFDouble) ? header.Missing.Float : MFDefaultMissingFloat;
    if (outFileName == (char *) NULL) outFile = stdout;
    else {
        if (strncmp(CMfileExtension(outFileName), "gz", 2) == 0) {
            char pCommand[strlen(outFileName) + 16];
            sprintf(pCommand, "gzip > %s", outFileName);
            outFile = popen(pCommand, "w");
            outCompress = true;
        }
        else {
            outFile = fopen(outFileName, "w");
            outCompress  = false;
        }
        if (outFile == (FILE *) NULL) {
            CMmsgPrint (CMmsgUsrError,"Output file opening error: %s",outFileName);
            goto Stop;
        }
    }
    for (recordID = 0; recordID < recordNum; ++recordID) {
        strcpy(outHeader.Date, dates[recordID]);
        for (i = 0; i < itemNum; ++i)
            arrays[recordID][i] = obsNums[recordID][i] > 0 ? arrays[recordID][i] / ((double) obsNums[recordID][i]) : outHeader.Missing.Float; 
        if ((MFdsHeaderWrite(&outHeader, outFile) != CMsucceeded) ||
            (fwrite(arrays[recordID], sizeof(float), itemNum, outFile) != itemNum)) {
            CMmsgPrint(CMmsgSysError, "Output writing error in: %s:%d", __FILE__, __LINE__);
            goto Stop;
        }
    }
    ret = CMsucceeded;
Stop:
    if ((ret == CMfailed) && (inFile != stdin) && (inFile != (FILE *) NULL))  { if (inCompress) pclose (inFile); else fclose(inFile); }
    if (items   != (void *)   NULL) free(items);
    if (arrays  != (float **) NULL) { for (recordID = 0; recordID < recordNum; ++recordID) free (arrays[recordID]);  free(arrays);  }
    if (dates   != (char **)  NULL) { for (recordID = 0; recordID < recordNum; ++recordID) free (dates[recordID]);   free(dates);   }
    if (obsNums != (int **)   NULL) { for (recordID = 0; recordID < recordNum; ++recordID) free (obsNums[recordID]); free(obsNums); }
  	if (outFile != stdout) { if (outCompress) pclose (outFile); else fclose(outFile); }
    return (ret);
}
