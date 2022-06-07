/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDnetcdf2ds.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <RG.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <input netcdf> <output datastream>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -m, --template     <template coverage>");
    CMmsgPrint(CMmsgInfo, "     -P, --processor [number]");
    CMmsgPrint(CMmsgInfo, "     -R, --report    [off|on]");
    CMmsgPrint(CMmsgInfo, "     -h, --help");
}

int main(int argc, char *argv[]) {
    DBInt argPos, argNum = argc, ret = CMfailed, report = false;
    CMthreadTeam_p team = (CMthreadTeam_p) NULL;

    FILE *outFile  = (FILE *) NULL;
    char *tmplName = (char *) NULL, *fieldName = (char *) NULL;
    DBObjData *grdData = (DBObjData *) NULL, *tmplData = (DBObjData *) NULL;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-m", "--template")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network!");
                goto Stop;
            }
            tmplName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-P", "--processor")) {
            DBInt procNum;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing processor number!");
                goto Stop;
            }
            if (sscanf (argv[argPos],"%d",&procNum) != 1) {
                 CMmsgPrint(CMmsgUsrError, "Invalid processor number!");
                goto Stop;             
            }
            if ((team = CMthreadTeamCreate (procNum)) == (CMthreadTeam_p) NULL) {
                CMmsgPrint (CMmsgUsrError,"Team initialization error %s, %d",__FILE__,__LINE__);
                goto Stop;
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-R", "--report")) {
            bool opsCodes[] = {false, true};
            const char *opsStrs[] = {"off", "on", (char *) NULL};

            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing aggregation method!");
                return (CMfailed);
            }
            if ((report = CMoptLookup(opsStrs, argv[argPos], true)) == DBFault) {
                CMmsgPrint(CMmsgUsrError, "Invalid aggregation method!");
                return (CMfailed);
            }
            report = opsCodes[report];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
Help:   if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage(argv[0]);
            ret = DBSuccess;
            goto Stop;
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            goto Stop;
        }
        argPos++;
    }

    if (argNum < 2) {
        CMmsgPrint(CMmsgUsrError, "Missing input file");
        _CMDprintUsage (argv[0]);
        goto Stop;
    }
    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        _CMDprintUsage (argv[0]);
        goto Stop;
    }
    if ((team == (CMthreadTeam_p) NULL) && ((team = CMthreadTeamCreate (CMthreadProcessorNum ())) == (CMthreadTeam_p) NULL)) {
            CMmsgPrint (CMmsgUsrError,"Team initialization error %s, %d",__FILE__,__LINE__);
            goto Stop;
    }

    outFile = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? fopen(argv[2], "w") : stdout;
    if (outFile == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "Output file Opening error in: %s %d", __FILE__, __LINE__);
        goto Stop;
    }

    grdData = new DBObjData("Noname", DBTypeGridContinuous);

    ret = DBImportNetCDF(grdData, argv[1]);

    if ((ret == DBFault) || ((grdData->Type() & DBTypeGrid) != DBTypeGrid)) {
        goto Stop;
    }

    if (tmplName != (char *) NULL) {
        tmplData = new DBObjData();
        if (tmplData->Read(tmplName) == DBFault) goto Stop;
    }

    ret = RGlibRGIS2DataStream(grdData, tmplData, outFile, team);
Stop:
    if (tmplData != (DBObjData *) NULL) delete tmplData;
    if (grdData  != (DBObjData *) NULL) delete grdData;
    if ((outFile != (FILE *) NULL) && (outFile != stdout)) fclose(outFile);
    if (team != (CMthreadTeam_p) NULL) {
        if (report) CMthreadTeamPrintReport (CMmsgInfo, team);
        CMthreadTeamDelete (team);
    }
    return (ret);
}
