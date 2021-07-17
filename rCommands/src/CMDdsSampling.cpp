/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDdsSampling.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <stdlib.h>
#include <string.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgUsrError, "%s [options] <in datastream> <out datastream>", CMfileName(arg0));
    CMmsgPrint(CMmsgUsrError, "  -i, --item [item]");
    CMmsgPrint(CMmsgUsrError, "  -h,--help");
}

int main(int argc, char *argv[]) {
    int argPos = 0, argNum = argc, ret = CMfailed, itemSize, itemID;
    FILE *inFile = stdin, *outFile = stdout;
    void *items = (void *) NULL;
    MFdsHeader_t header;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest(argv[argPos], "-i", "--item")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing sampling item!");
                return (CMfailed);
            }
            if ((sscanf(argv[argPos], "%d", &itemID)) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid sampling item");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        Help:
        if (CMargTest(argv[argPos], "-h", "--help")) {
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
    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        _CMDprintUsage (argv[0]);
        goto Stop;
    }

    if ((inFile = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? fopen(argv[1], "r") : stdin) == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "Input file opening error in: %s %d", __FILE__, __LINE__);
        goto Stop;
    }
    if ((outFile = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? fopen(argv[2], "w") : stdout) == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "Output file opening error in: %s %d", __FILE__, __LINE__);
        goto Stop;
    }

    while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
        if (items == (void *) NULL) {
            itemSize = MFVarItemSize(header.Type);
            if ((items = (void *) calloc(header.ItemNum, itemSize)) == (void *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
        }
        if ((int) fread(items, itemSize, header.ItemNum, inFile) != header.ItemNum) {
            CMmsgPrint(CMmsgSysError, "Input reading error in: %s:%d", __FILE__, __LINE__);
            goto Stop;
        }
        if ((itemID < 0) || (itemID >= header.ItemNum)) {
            CMmsgPrint(CMmsgAppError, "Invalid Item id [%d]", itemID);
            continue;
        }
        switch (header.Type) {
            case MFByte:
                if (((char *) items)[itemID] != header.Missing.Int)
                    fprintf(outFile, "%s\t%d\n", header.Date, (int) ((char *) items)[itemID]);
                else
                    fprintf(outFile, "%s\t\n", header.Date);
                break;
            case MFShort:
                if (header.Swap != 1) MFSwapHalfWord(((short *) items) + itemID);
                if (((short *) items)[itemID] != header.Missing.Int)
                    fprintf(outFile, "\t%d\n", (int) ((short *) items)[itemID]);
                else
                    fprintf(outFile, "%s\t\n", header.Date);
                break;
            case MFInt:
                if (header.Swap != 1) MFSwapWord(((int *) items) + itemID);
                if (((int *) items)[itemID] != header.Missing.Int)
                    fprintf(outFile, "%s\t%d\n", header.Date, (int) ((int *) items)[itemID]);
                else
                    fprintf(outFile, "%s\t\n", header.Date);
                break;
            case MFFloat:
                if (header.Swap != 1) MFSwapWord(((float *) items) + itemID);
                if (CMmathEqualValues(((float *) items)[itemID], header.Missing.Float) == false)
                    fprintf(outFile, "%s\t%f\n", header.Date, (float) ((float *) items)[itemID]);
                else
                    fprintf(outFile, "%s\t\n", header.Date);
                break;
            case MFDouble:
                if (header.Swap != 1) MFSwapLongWord(((double *) items) + itemID);
                if (CMmathEqualValues(((double *) items)[itemID], header.Missing.Float) == false)
                    fprintf(outFile, "%s\t%lf\n", header.Date, (double) ((double *) items)[itemID]);
                else
                    fprintf(outFile, "%s\t\n", header.Date);
                break;
        }
    }
    if (ferror (inFile) != 0) {
        CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
        ret = CMfailed;
    }
    else ret = CMsucceeded;
Stop:
    if (inFile != stdin) fclose(inFile);
    if (outFile != stdout) fclose(outFile);
    return (ret);
}
