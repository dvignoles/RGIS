/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDpntSTNCoordinate.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <RG.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <input point> <output point>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -n,--network   <network coverage>");
    CMmsgPrint(CMmsgInfo, "     -f,--field     [source field]");
    CMmsgPrint(CMmsgInfo, "     -c,--cfield    [compare field]");
    CMmsgPrint(CMmsgInfo, "     -T,--tolerance [error tolerance in percent]");
    CMmsgPrint(CMmsgInfo, "     -R,--radius    [radius]");
    CMmsgPrint(CMmsgInfo, "     -M,--mode      [adaptive|fixed]");
    CMmsgPrint(CMmsgInfo, "     -t,--title     [dataset title]");
    CMmsgPrint(CMmsgInfo, "     -u,--subject   [subject]");
    CMmsgPrint(CMmsgInfo, "     -d,--domain    [domain]");
    CMmsgPrint(CMmsgInfo, "     -v,--version   [version]");
    CMmsgPrint(CMmsgInfo, "     -V,--verbose");
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    char *pFieldName = (char *) NULL, *cFieldName = DBrNSubbasinArea;
    char *title = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    char *networkName = (char *) NULL;
    bool adaptive = true;
    DBFloat radius    = 10.0;
    DBFloat tolerance = 10.0;
    DBObjData *data, *netData;
    DBObjTable *pTable, *cTable;
    DBObjTableField *pField, *cField;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-n", "--network")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing network coverage!");
                return (CMfailed);
            }
            networkName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-f", "--field")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing fieldname!");
                return (CMfailed);
            }
            pFieldName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-c", "--cField")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing fieldname!");
                return (CMfailed);
            }
            cFieldName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-T", "--tolerance")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing limit!");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%lf",&tolerance) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid limit!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-R", "--radius")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing pixel radius!");
                return (CMfailed);
            }
            if (sscanf (argv[argPos],"%lf", &radius) != 1) {
                CMmsgPrint(CMmsgUsrError, "Invalid pixel radius!");
                return (CMfailed);
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-M", "--method")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing radius method!");
                return (CMfailed);
            }
            else {
                const char *options[] = {"adaptive", "fixed", (char *) NULL};
                bool methods[] = { true, false};
                DBInt code;

                if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                    CMmsgPrint(CMmsgWarning, "Ignoring illformed radius method [%s]!", argv[argPos]);
                }
                else adaptive = methods[code];
            }
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
    if (networkName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Network coverage is not specified");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }

    if (verbose) RGlibPauseOpen(argv[0]);

    netData = new DBObjData();
    if ((netData->Read(networkName) == DBFault) || (netData->Type() != DBTypeNetwork)) {
        CMmsgPrint(CMmsgUsrError, "Invalid network coverage!");
        _CMDprintUsage (argv[0]);
        delete netData;
        return (CMfailed);
    }
    cTable = netData->Table(DBrNCells);

    data = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? data->Read(argv[1]) : data->Read(stdin);
    if ((ret == DBFault) || (data->Type() != DBTypeVectorPoint)) {
        CMmsgPrint(CMmsgUsrError, "Invalid point coverage!");
        _CMDprintUsage (argv[0]);
        delete netData;
        delete data;
        return (CMfailed);
    }
    pTable = data->Table(DBrNItems);
    if (title != (char *) NULL) data->Name(title);
    if (subject != (char *) NULL) data->Document(DBDocSubject, subject);
    if (domain != (char *) NULL) data->Document(DBDocGeoDomain, domain);
    if (version != (char *) NULL) data->Document(DBDocVersion, version);

    data->LinkedData(netData);
    pField = pFieldName != (char *) NULL ? pTable->Field(pFieldName) : (DBObjTableField *) NULL;
    cField = cTable->Field(cFieldName);
    if ((ret = RGlibPointSTNCoordinates (data, pField, cField, tolerance / 100.0, radius, adaptive)) == DBSuccess)
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? data->Write(argv[2]) : data->Write(stdout);

    delete netData;
    delete data;
    if (verbose) RGlibPauseClose();
    return (ret);
}
