/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

CMDvdbLookup.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <cm.h>
#include <vdb.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options]",arg0);
    CMmsgPrint(CMmsgInfo, "     -c,--cfname      [RGIS name]");
    CMmsgPrint(CMmsgInfo, "     -g,--rgisname    [CF name]");
    CMmsgPrint(CMmsgInfo, "     -l,--longname    [variable name]");
    CMmsgPrint(CMmsgInfo, "     -a,--aggregation [variable name]");
    CMmsgPrint(CMmsgInfo, "     -s,--shadeset    [variable name]");
    CMmsgPrint(CMmsgInfo, "     -t,--datatype    [variable name]");
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    const char *cfName = (const char *) NULL;
    const char *rgName = (const char *) NULL;
    const char *lgName = (const char *) NULL;
    const char *dtType = (const char *) NULL;
    const char *shdSet = (const char *) NULL;
    const char *aggreg = (const char *) NULL;

    if (argNum != 3) {
        _CMDprintUsage (argv[0]);
        return (CMsucceeded);
    }

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-c", "--cfname")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing RGIS name!");
                return (CMfailed);
            }
            rgName = (const char *) argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-g", "--rgisname")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing CF name!");
                return (CMfailed);
            }
            cfName = (const char *) argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-l", "--longname")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing variable name!");
                return (CMfailed);
            }
            lgName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-t", "--datatype")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing variable name!");
                return (CMfailed);
            }
            dtType = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-s", "--shadeset")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing variable name!");
                return (CMfailed);
            }
            shdSet = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-a", "--aggregation")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing variable name!");
                return (CMfailed);
            }
            aggreg = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage (argv[0]);
            return (CMsucceeded);
        }
        if ((argv[argPos][0] == '-') && ((int) strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    if       (cfName != (const char *) NULL) {
        if ((rgName = VDBrgName (cfName)) != (const char *) NULL) printf ("%s",rgName);
        return (0);
    } else if (rgName != (const char*) NULL) {
        if ((cfName = VDBcfName (rgName)) != (const char *) NULL) printf ("%s",cfName);
    } else if (lgName != (const char *) NULL) {
        cfName = lgName;
        if (((lgName = VDBlongName(cfName)) != NULL) ||
            (((cfName = VDBcfName(cfName)) != NULL) && ((lgName = VDBlongName(cfName)) != NULL)))
            printf("%s",lgName);
    } else if (dtType != (const char *) NULL) {
        cfName = dtType;
        if (((dtType = VDBdataType(cfName)) != NULL) ||
            (((cfName = VDBcfName(cfName)) != NULL) && ((dtType = VDBdataType(cfName)) != NULL)))
            printf("%s",dtType);
    } else if (shdSet != (const char *) NULL) {
        cfName = shdSet;
        if (((shdSet = VDBshadset(cfName)) != NULL) ||
            (((cfName = VDBcfName(cfName)) != NULL) && ((shdSet = VDBshadset(cfName)) != NULL)))
            printf("%s",shdSet);
    } else if (aggreg != (const char *) NULL) {
        cfName = aggreg;
        if (((aggreg = VDBaggregation(cfName)) != NULL) ||
            (((cfName = VDBcfName(cfName)) != NULL) && ((aggreg = VDBaggregation(cfName)) != NULL)))
            printf("%s",aggreg);
    }
    return (ret);
}