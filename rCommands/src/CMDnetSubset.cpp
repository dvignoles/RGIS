/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDnetSubset.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <DBif.hpp>
#include <RG.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <input network> <output network>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -p,--points      <point coverage>");
    CMmsgPrint(CMmsgInfo, "     -t,--title       [dataset title]");
    CMmsgPrint(CMmsgInfo, "     -d,--domain      [domain]");
    CMmsgPrint(CMmsgInfo, "     -v,--version     [version]");
    CMmsgPrint(CMmsgInfo, "     -V,--verbose");
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    char *title = (char *) NULL, *domain = (char *) NULL, *version = (char *) NULL;
    char *pointName = (char *) NULL;
    DBInt cellID, pntID;
    DBPosition pos;
    DBObjData *netData, *pntData;
    DBObjRecord *pntRec, *cellRec;
    DBObjTable  *cellTable, *saveTable;
    DBNetworkIF *netIF;
    DBVPointIF  *pntIF;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-p", "--points")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network coverage!");
                return (CMfailed);
            }
            pointName = argv[argPos];
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

    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if (pointName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Missing point coverage!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if (verbose) RGlibPauseOpen(argv[0]);

    pntData = new DBObjData ();
    if ((pntData->Read(pointName) == CMfailed) || (pntData->Type () != DBTypeVectorPoint)) {
        CMmsgPrint(CMmsgUsrError, "Invalid point coverage [%s]", pointName);
        _CMDprintUsage (argv[0]);
        delete pntData;
        return (CMfailed);
    }

    netData = new DBObjData ();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? netData->Read(argv[1]) : netData->Read(stdin);
    if ((ret == CMfailed) || (netData->Type() != DBTypeNetwork)) {
        CMmsgPrint(CMmsgUsrError, "Invalid network input!");
        _CMDprintUsage (argv[0]);
        delete netData;
        return (CMfailed);
    }
    if (title   != (char *) NULL) netData->Name(title);
    if (domain  != (char *) NULL) netData->Document(DBDocGeoDomain, domain);
    if (version != (char *) NULL) netData->Document(DBDocVersion, version);

    netIF = new DBNetworkIF(netData);
    pntIF = new DBVPointIF (pntData);

    cellTable = netData->Table(DBrNCells);
    for (cellID = 0; cellID < cellTable->ItemNum (); cellID++) {
        cellRec = cellTable->Item (cellID);
        cellRec->Flags(DBObjectFlagSelected, DBSet);
    }
    for (pntID = 0;pntID < pntIF->ItemNum(); pntID++) {
        pntRec = pntIF->Item(pntID);
        if ((cellRec = netIF->Cell (pntIF->Coordinate(pntRec))) == (DBObjRecord *) NULL) continue;
        netIF->UpStreamSearch	(cellRec, DBNetworkUnselect);
    }
    saveTable = new DBObjTable (*cellTable);

    for (cellID = 0;cellID < cellTable->ItemNum(); ++cellID) {
        cellRec = cellTable->Item(cellID);
        netIF->CellDelete(cellRec);
    }
    cellTable->DeleteAll();
    for (cellID = 0; cellID < saveTable->ItemNum(); ++cellID) {
        cellRec = saveTable->Item(cellID);
        if ((cellRec->Flags() & DBObjectFlagSelected) != DBObjectFlagSelected)
            cellTable->Add (new DBObjRecord(*cellRec));
    }
    for (cellID = 0;cellID < cellTable->ItemNum(); ++cellID) {
        cellRec = cellTable->Item(cellID);
        netIF->CellAdd(cellRec);
    }
    delete saveTable;
    if ((netIF->Build () == CMsucceeded) &&
        (netIF->Trim  () == CMsucceeded) &&
        (netIF->Build () == CMsucceeded)) {
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? netData->Write(argv[2]) : netData->Write(stdout);
    }
    delete pntIF;
    delete netIF;
    delete pntData;
    delete netData;
    if (verbose) RGlibPauseClose();
    return (ret);
}
