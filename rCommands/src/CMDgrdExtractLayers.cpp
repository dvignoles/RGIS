/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDgrdExtractLayers.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include<cm.h>
#include<DB.hpp>
#include<DBif.hpp>
#include<RG.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <input grid> <output grid>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -f,--first     [layername]");
    CMmsgPrint(CMmsgInfo, "     -l,--last      [layername]");
    CMmsgPrint(CMmsgInfo, "     -t,--title     [dataset title]");
    CMmsgPrint(CMmsgInfo, "     -u,--subject   [subject]");
    CMmsgPrint(CMmsgInfo, "     -d,--domain    [domain]");
    CMmsgPrint(CMmsgInfo, "     -v,--version   [version]");
    CMmsgPrint(CMmsgInfo, "     -V,--verbose");
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    char *title = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    char *firstLayer = (char *) NULL, *lastLayer = (char *) NULL;
    DBObjData *grdData;
    DBObjRecord *layerRec;
    DBGridIF *gridIF;

    if (argNum < 2) goto Help;
 
    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-f", "--first")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing first layer!");
                return (CMfailed);
            }
            firstLayer = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-l", "--last")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing last layer!");
                return (CMfailed);
            }
            lastLayer = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-t", "--title")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing title!");
                return (CMfailed);
            }
            title = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-u", "--subject")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing subject!");
                return (CMfailed);
            }
            subject = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-d", "--domain")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing domain!");
                return (CMfailed);
            }
            domain = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-v", "--version")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing version!");
                return (CMfailed);
            }
            version = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
Help:   if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage(argv[0]);
            return (DBSuccess);
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
    if (verbose) RGlibPauseOpen(argv[0]);

    grdData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? grdData->Read(argv[1]) : grdData->Read(stdin);
    if ((ret == DBFault) || ((grdData->Type() & DBTypeGrid) != DBTypeGrid)) {
        delete grdData;
        return (CMfailed);
    }

    gridIF = new DBGridIF(grdData);

    if (firstLayer != (char *) NULL)
        gridIF->DeleteLayers((gridIF->Layer(0))->Name(), firstLayer);
    if (lastLayer != (char *) NULL) {
        if ((layerRec = gridIF->Layer(lastLayer)) == (DBObjRecord *) NULL) {
            CMmsgPrint(CMmsgUsrError, "Invalid last layer!");
            delete gridIF;
            delete grdData;
            return (CMfailed);
        }
        if (layerRec->RowID() < gridIF->LayerNum() - 1) {
            layerRec = gridIF->Layer(layerRec->RowID() + 1);
            gridIF->DeleteLayers(layerRec->Name(), (gridIF->Layer(gridIF->LayerNum() - 1))->Name());
            gridIF->DeleteLayer(gridIF->Layer(gridIF->LayerNum() - 1)->Name());
        }
    }
    delete gridIF;

    if (title != (char *) NULL) grdData->Name(title);
    if (subject != (char *) NULL) grdData->Document(DBDocSubject, subject);
    if (domain != (char *) NULL) grdData->Document(DBDocGeoDomain, domain);
    if (version != (char *) NULL) grdData->Document(DBDocVersion, version);

    ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? grdData->Write(argv[2]) : grdData->Write(stdout);

    delete grdData;
    if (verbose) RGlibPauseClose();
    return (ret);
}
