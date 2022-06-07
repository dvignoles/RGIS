/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDdsAggregate.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <stdlib.h>
#include <string.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgUsrError, "%s [options] <in datastream> <out datastream>", CMfileName(arg0));
    CMmsgPrint(CMmsgUsrError, "  -e, --step [day|month|year]");
    CMmsgPrint(CMmsgUsrError, "  -a, --aggregate [avg|sum]");
    CMmsgPrint(CMmsgUsrError, "  -h,--help");
}

enum { DAY = 10, MONTH = 7, YEAR = 4 };

int main(int argc, char *argv[]) {
    int argPos = 0, argNum = argc, ret = CMfailed, itemSize, itemRet, i, recordNum = 0, step = DAY, average = true;
    FILE *inFile = stdin, *outFile = stdout;
    char date[MFDateStringLength];
    MFdsHeader_t header, outHeader;
    void *items = (void *) NULL;
    double *array = (double *) NULL;
    float *record = (float *) NULL;
    int *obsNum = (int *) NULL, maxObs = 0;

    if (argNum < 2) goto Help;
    date[0] = '\0';

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest(argv[argPos], "-e", "--step")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing time step!");
                break;
            }
            else {
                const char *options[] = {"day", "month", "year", (char *) NULL};
                int codes[] = {DAY, MONTH, YEAR}, code;

                if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                    CMmsgPrint(CMmsgWarning, "Ignoring illformed step option [%s]!", argv[argPos]);
                }
                else step = codes[code];
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-a", "--aggregate")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing aggregate option!");
                break;
            }
            else {
                const char *options[] = {"avg", "sum", (char *) NULL};
                int codes[] = {true, false}, code;

                if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                    CMmsgPrint(CMmsgWarning, "Ignoring illformed step option [%s]!", argv[argPos]);
                }
                else average = codes[code];
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
        return (CMfailed);
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
            if ((array = (double *) calloc(header.ItemNum, sizeof(double))) == (double *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
            if ((record = (float *) calloc(header.ItemNum, sizeof(float))) == (float *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
            if ((obsNum = (int *) calloc(header.ItemNum, sizeof(int))) == (int *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
            for (i = 0; i < header.ItemNum; i++) obsNum[i] = 0;
            for (i = 0; i < header.ItemNum; i++) array[i] = 0.0;
            maxObs = 0;
            outHeader.Swap = 1;
            outHeader.Type = MFFloat;
            outHeader.ItemNum = header.ItemNum;
            outHeader.Missing.Float = (header.Type == MFFloat) || (header.Type == MFDouble) ? header.Missing.Float : MFDefaultMissingFloat;
        }
        if (fread(items, itemSize, header.ItemNum, inFile) != header.ItemNum) {
            CMmsgPrint(CMmsgSysError, "Input reading error in: %s:%d", __FILE__, __LINE__);
            goto Stop;
        }
        if (recordNum == 0) strncpy (date, header.Date, step);
        else {
            if (strncmp (date, header.Date, step) != 0) {
                strcpy(outHeader.Date, date);
                if (MFdsHeaderWrite (&outHeader, outFile) == CMfailed)  {
                    CMmsgPrint(CMmsgSysError, "Output header writing error in: %s:%d", __FILE__, __LINE__);
                    goto Stop;
                }
                if (average)
                    for (i = 0; i < header.ItemNum; i++) record[i] = obsNum[i] > 0 ? array[i] / (double) obsNum[i] : outHeader.Missing.Float;
                else 
                    for (i = 0; i < header.ItemNum; i++) record[i] = obsNum[i] > 0 ? array[i] * ((double) maxObs / (double) obsNum[i]) : outHeader.Missing.Float;
                if ((int) fwrite(record, sizeof(float), outHeader.ItemNum, outFile) != outHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Output writing error in: %s:%d", __FILE__, __LINE__);
                    goto Stop;
                }
                for (i = 0; i < header.ItemNum; i++) obsNum[i] = 0;
                for (i = 0; i < header.ItemNum; i++) array[i]  = 0.0;
                maxObs = 0;
                strncpy (date, header.Date, step);
            }
        } 
        for (i = 0; i < header.ItemNum; i++) {
            switch (header.Type) {
                case MFByte:
                    if (((char *) items)[i] != header.Missing.Int) {
                        array  [i] += ((char *) items)[i];
                        obsNum [i] += 1;
                        if (obsNum[i] > maxObs) maxObs = obsNum[i];
                    }
                    break;
                case MFShort:
                    if (header.Swap != 1) MFSwapHalfWord(((short *) items) + i);
                    if (((short *) items)[i] != header.Missing.Int) {
                        array  [i] += ((short *) items)[i];
                        obsNum [i] += 1;
                        if (obsNum[i] > maxObs) maxObs = obsNum[i];
                    }
                    break;
                case MFInt:
                    if (header.Swap != 1) MFSwapWord(((int *) items) + i);
                    if (((int *) items)[i] != header.Missing.Int) {
                        array  [i] += ((int *) items)[i];
                        obsNum [i] += 1;
                        if (obsNum[i] > maxObs) maxObs = obsNum[i];
                    }
                    break;
                case MFFloat:
                    if (header.Swap != 1) MFSwapWord(((float *) items) + i);
                    if (CMmathEqualValues(((float *) items)[i], header.Missing.Float) == false) {
                        array  [i] += ((float *) items)[i];
                        obsNum [i] += 1;
                        if (obsNum[i] > maxObs) maxObs = obsNum[i];
                    }
                    break;
                case MFDouble:
                    if (header.Swap != 1) MFSwapLongWord(((double *) items) + i);
                    if (CMmathEqualValues(((double *) items)[i], header.Missing.Float) == false) {
                        array  [i] += ((double *) items)[i];
                        obsNum [i] += 1;
                        if (obsNum[i] > maxObs) maxObs = obsNum[i];
                    }
                    break;
            }
        }
        recordNum++;
    }
    if (recordNum > 0) {
        strcpy(outHeader.Date, date);
        if (MFdsHeaderWrite (&outHeader, outFile) == CMfailed)  {
            CMmsgPrint(CMmsgSysError, "Output header writing error in: %s:%d", __FILE__, __LINE__);
            goto Stop;
        }
        if (average)
            for (i = 0; i < header.ItemNum; i++) record[i] = obsNum[i] > 0 ? array[i] / obsNum[i] : outHeader.Missing.Float;
        else
            for (i = 0; i < header.ItemNum; i++) record[i] = obsNum[i] > 0 ? array[i] * ((double) maxObs / (double) obsNum[i]) : outHeader.Missing.Float;
        if ((int) fwrite(record, sizeof(float), outHeader.ItemNum, outFile) != outHeader.ItemNum) {
            CMmsgPrint(CMmsgSysError, "Output writing error in: %s:%d", __FILE__, __LINE__);
            goto Stop;
        }
        ret = CMsucceeded;
    }
Stop:
    if (items   != (void *)   NULL) free(items);
    if (array   != (double *) NULL) free(array);
    if (obsNum  != (int *)    NULL) free(obsNum);
    if (inFile  != stdin)  fclose(inFile);
    if (outFile != stdout) fclose(outFile);
    return (ret);
}