/******************************************************************************

GHAAS RiverGIS Utilities V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

CMDtblAddIdXY.C

zachary.tessler@asrc.cuny.edu
bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.H>
#include <DBif.H>
#include <RG.H>

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, verbose = false;
    DBInt recID;
    DBObjData *data;
    char *tableName = (char *) NULL;
    char *fieldIDName = (char *) NULL;
    char *fieldXName = (char *) NULL;
    char *fieldYName = (char *) NULL;
    DBObjTable *table;
    DBObjTableField *fieldX, *fieldY;
    DBObjRecord *record;
    DBNetworkIF *netIF;
    DBVPointIF  *pntIF;
	DBCoordinate coord;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-a", "--table")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing table name!");
                return (CMfailed);
            }
            tableName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-x", "--Xfield")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing field name!");
                return (CMfailed);
            }
            fieldXName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-y", "--Yfield")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing field name!");
                return (CMfailed);
            }
            fieldYName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-V", "--verbose")) {
            verbose = true;
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-h", "--help")) {
            CMmsgPrint(CMmsgInfo, "%s [options] <input file> <output file>", CMfileName(argv[0]));
            CMmsgPrint(CMmsgInfo, "     -a, --table   [ [DBItems | DBCells]]");
            CMmsgPrint(CMmsgInfo, "     -x, --Xfield  <field name>");
            CMmsgPrint(CMmsgInfo, "     -y, --Yfield  <field name>");
            CMmsgPrint(CMmsgInfo, "     -V, --verbose");
            CMmsgPrint(CMmsgInfo, "     -h, --help");
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
        return (CMfailed);
    }
    if (verbose) RGlibPauseOpen(argv[0]);

    data = new DBObjData();
    if (((argNum > 1) && (strcmp(argv[1], "-") != 0) ? data->Read(argv[1]) : data->Read(stdin)) == DBFault) {
        delete data;
        return (CMfailed);
    }

    if (tableName == (char *) NULL) tableName = (char *) "DBItems";
    if ((table = data->Table(tableName)) == (DBObjTable *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Invalid table: %s!", tableName);
        delete data;
        return (CMfailed);
    }

    switch (data->Type()) {
        case DBTypeVectorPoint:
            pntIF = new DBVPointIF (data);
            if (strcmp("tableName","DBItems") == 0 ) {

                if (fieldXName == (char *) NULL)  fieldXName = (char *) "PointXCoord";
                if (fieldYName == (char *) NULL)  fieldYName = (char *) "PointYCoord";
                fieldX  = new DBObjTableField (fieldXName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                fieldY  = new DBObjTableField (fieldYName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                table->AddField(fieldX);
                table->AddField(fieldY);
                for (recID = 0; recID < table->ItemNum(); ++recID) {
                    record = pntIF->Item(recID);
                    coord  = pntIF->Coordinate (record);
                    fieldX->Float(record,coord.X);
                    fieldY->Float(record,coord.Y);
                }
            } else {
                CMmsgPrint(CMmsgUsrError, "Invalid tablename (this cannot happen): %s!", tableName);
                ret = CMfailed;
            }

            break;
        case DBTypeNetwork:
            netIF = new DBNetworkIF(data);
            if (strcmp(tableName,"DBItems") == 0) {
                if (fieldXName == (char *) NULL)  fieldXName = (char *) "MouthXCoord";
                if (fieldYName == (char *) NULL)  fieldYName = (char *) "MouthYCoord";
                fieldX  = new DBObjTableField (fieldXName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                fieldY  = new DBObjTableField (fieldYName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                table->AddField(fieldX);
                table->AddField(fieldY);
                for (recID = 0; recID < table->ItemNum(); ++recID) {
                    record = netIF->Basin(recID);
                    coord = netIF->Center(netIF->MouthCell(record));
                    fieldX->Float(record,coord.X);
                    fieldY->Float(record,coord.Y);
                }
                ret = CMsucceeded;
            } else if (strcmp(tableName,"DBCells") == 0) {
                if (fieldXName == (char *) NULL)  fieldXName = (char *) "CellXCoord";
                if (fieldYName == (char *) NULL)  fieldYName = (char *) "CellYCoord";
                fieldX  = new DBObjTableField (fieldXName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                fieldY  = new DBObjTableField (fieldYName, DBTableFieldFloat, (char *) "%10.3f", sizeof (DBFloat4));
                table->AddField(fieldX);
                table->AddField(fieldY);
                for (recID = 0; recID < table->ItemNum(); ++recID) {
                    record = netIF->Cell(recID);
                    coord  = netIF->Center(record);
                    fieldX->Float(record, coord.X);
                    fieldY->Float(record, coord.Y);
                }
                ret = CMsucceeded;
            } else {
                CMmsgPrint(CMmsgUsrError, "Invalid tablename (this cannot happen): %s!", tableName);
                ret = CMfailed;
            }
            delete netIF;
            break;
        default: CMmsgPrint(CMmsgUsrError, "Not point or network!"); break;
    }

    if (ret == CMsucceeded)
        ret = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? data->Write(argv[2]) : data->Write(stdout);
    delete data;
    if (verbose) RGlibPauseClose();
    return (ret);
}
