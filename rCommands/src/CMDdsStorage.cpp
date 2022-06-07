/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDdsStorage.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <string.h>
#include <stdlib.h>

class CMDgrdStorage {
    private:
        size_t  TargetRecordNum, TargetItemSize, ItemNum;
        void  *Buffer;
        float *CumulSum;
        float *MaxSum;
    public:
        int Initialize (char *target) {
            size_t itemID;
            Buffer    = (void *)  NULL;
            CumulSum  = (float *) NULL;
            MaxSum    = (float *) NULL;
            TargetRecordNum = 0, TargetItemSize = 0;
            MFDataStream_t TargetDS;
            MFDataStream_t SourceDS;
            MFdsHeader_p TargetHeader = (MFdsHeader_p) NULL;
            MFdsHeader_t SourceHeader;
            TargetDS.Type        = SourceDS.Type        = MFFile;
            TargetDS.Handle.File = SourceDS.Handle.File = (FILE *) NULL;

            if ((TargetDS.Handle.File = fopen (target,"r")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgSysError, "Target file opening error: %s %d", __FILE__, __LINE__);
                return (CMfailed);
            }
            while (MFdsHeaderRead (&SourceHeader, TargetDS.Handle.File) == CMsucceeded) {
                if ((TargetHeader = (MFdsHeader_p) reallocf (TargetHeader, (TargetRecordNum + 1) * sizeof (MFdsHeader_t))) == (MFdsHeader_p) NULL) {
                    CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                    return (CMfailed);
                }
                if ((memcpy (TargetHeader + TargetRecordNum, &SourceHeader, sizeof (MFdsHeader_t))) != TargetHeader + TargetRecordNum) {
                    CMmsgPrint(CMmsgSysError, "Memory copying error in: %s %d", __FILE__, __LINE__);
                    return (CMfailed);
                }
                switch (TargetHeader[TargetRecordNum].Type) {
                    case MFByte:   TargetItemSize = sizeof (char);   break;
                    case MFShort:  TargetItemSize = sizeof (short);  break;
                    case MFInt:    TargetItemSize = sizeof (int);    break;
                    case MFFloat:  TargetItemSize = sizeof (float);  break;
                    case MFDouble: TargetItemSize = sizeof (double); break;
                }
                if ((Buffer = reallocf (Buffer, (TargetRecordNum + 1) * TargetHeader[TargetRecordNum].ItemNum * TargetItemSize)) == (void *) NULL) {
                    CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                    return (CMfailed);
                }
                if ((fread ((char *) Buffer + (TargetRecordNum) * TargetHeader[TargetRecordNum].ItemNum * TargetItemSize,
                     TargetItemSize, TargetHeader[TargetRecordNum].ItemNum, TargetDS.Handle.File)) != TargetHeader[TargetRecordNum].ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Target file reading error: %s %d", __FILE__, __LINE__);
                    return (CMfailed);
                }
                TargetRecordNum++;
            }
            if (TargetRecordNum == 0) {
                CMmsgPrint(CMmsgSysError, "Missing target records: %s %d", __FILE__, __LINE__);
                return (CMfailed);
            }
            ItemNum = TargetHeader [0].ItemNum;
            if ((CumulSum = (float *) calloc (ItemNum,sizeof (float))) == (float *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                return (CMfailed);
            }
            if ((MaxSum   = (float *) calloc (ItemNum,sizeof (float))) == (float *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                return (CMfailed);
            }
            for (itemID = 0; itemID < ItemNum; ++itemID) CumulSum[itemID] = MaxSum[itemID] = 0;
            return (CMsucceeded);
        }
        int Run () {
            return (CMsucceeded);
        }
        int Finalize () {
            return (CMsucceeded);
        }
};

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <data stream 0> <input datastream 1> ... <data stream N>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo,     "     -T, --target    [target datastream]");
    CMmsgPrint(CMmsgUsrError, "     -m, --mode      [deficit|surplus]");
    CMmsgPrint(CMmsgInfo,     "     -o, --output    [data stream]");
    CMmsgPrint(CMmsgInfo,     "     -P, --processor [number]");
    CMmsgPrint(CMmsgInfo,     "     -R, --report    [off|on]");
    CMmsgPrint(CMmsgInfo,     "     -h, --help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, deficit = true;
    char *target = (char *) NULL, *output;
    CMDgrdStorage grdStorage;

    if (argNum < 2) goto Help;
 
    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-T", "--target")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing target grid!");
                return (CMfailed);
            }
            target = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-m", "--mode")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missging mode option!");
                break;
            }
            else {
                const char *options[] = {"deficit", "surplus", (char *) NULL};
                int codes[] = { true, false }, code;

                if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                    CMmsgPrint(CMmsgWarning, "Ignoring illformed step option [%s]!", argv[argPos]);
                }
                else deficit = codes[code];
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-o", "--output")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing merge grid!");
                return (CMfailed);
            }
            output = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
Help:   if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage(argv[0]);
            return (CMsucceeded);
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    if (target == (char *) NULL) { CMmsgPrint(CMmsgUsrError, "Missing target data!");      return (CMfailed); }
    if (argNum < 2)              { CMmsgPrint(CMmsgUsrError, "Missing time series data!"); return (CMfailed); }

    if (grdStorage.Initialize (target) == CMfailed) {

    }
    for (argPos = 2; argPos < argNum; ++argPos) {

    }
    ret = output != (char *) NULL ? CMsucceeded : CMfailed;
Stop:
    return (ret);
}
