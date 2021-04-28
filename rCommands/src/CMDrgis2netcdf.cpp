/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDrgis2netcdf.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <input data> <output netcdf>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    DBInt argPos, argNum = argc, ret;
    DBObjData *grdData;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage(argv[0]);
            return (DBSuccess);
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    if (argNum < 2) {
        CMmsgPrint(CMmsgUsrError, "Missing output netcdf!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }

    grdData = new DBObjData();
    ret = (argNum > 2) && (strcmp(argv[1], "-") != 0) ? grdData->Read(argv[1]) : grdData->Read(stdin);
    if (ret == DBFault) {
        delete grdData;
        return (CMfailed);
    }

    ret = DBExportNetCDF(grdData, argNum > 2 ? argv[2] : argv[1]);
    delete grdData;
    return (ret);
}
