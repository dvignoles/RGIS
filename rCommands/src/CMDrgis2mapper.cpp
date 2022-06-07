/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDrgis2mapper.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <DBif.hpp>
#include <MF.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <sampling data> <output mapper>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -d, --domain <rgis domain file>");
    CMmsgPrint(CMmsgInfo, "     -h, --help");
}

int main(int argc, char *argv[]) {
    FILE *outFile;
    DBInt argPos, argNum = argc, ret;
    int objID, sampleID;
    char *domain = (char *) NULL;
    MFMapper_p mapper = (MFMapper_p) NULL;
    DBCoordinate coord;
    DBObjData *domainData, *mapperData;
    DBNetworkIF *netIF;
    DBObjRecord *cellRec;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-d", "--domain")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing domain");
                return (CMfailed);
            }
            domain = argv[argPos];
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

    if (domain == (char *) NULL) {
         CMmsgPrint(CMmsgUsrError, "Missing domain name in: %s", CMfileName(argv[0]));
        _CMDprintUsage (argv[0]);
        return(DBFault);
    }

    domainData = new DBObjData ();
    if (domainData->Read(domain) == DBFault) {
         CMmsgPrint(CMmsgUsrError, "Domain [%s] reading error in: %s", domain, CMfileName(argv[0]));
        _CMDprintUsage (argv[0]);
        return(DBFault);
    }
    if (domainData->Type () != DBTypeNetwork) {
         CMmsgPrint(CMmsgUsrError, "Invalid domain data type in: %s", CMfileName(argv[0]));
        delete domainData;
        return(DBFault);
    }

    outFile = (argNum > 2) && (strcmp(argv[2], "-") != 0) ? fopen(argv[2], "w") : stdout;
    if (outFile == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Output file Opening error in: %s", CMfileName(argv[0]));
        _CMDprintUsage (argv[0]);
        delete domainData;
        return(DBFault);
    }

    mapperData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? mapperData->Read(argv[1]) : mapperData->Read(stdin);
    if (ret == DBFault) {
        CMmsgPrint(CMmsgUsrError, "Mapper reading error in: %s", CMfileName(argv[0]));
        _CMDprintUsage (argv[0]);
        delete domainData;
        if (outFile != stdout) fclose (outFile);
        return(DBFault);
    }

    if ((mapper = (MFMapper_p) calloc (1, sizeof(MFMapper_t))) != (MFMapper_p) NULL) {
        netIF = new DBNetworkIF (domainData);
        mapper->Swap = 1;
        mapper->ObjNum = netIF->CellNum ();
        if ((mapper->SampleIDs = (int *) calloc (mapper->ObjNum, sizeof (int))) == (int *) NULL) {
            CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: %s %d", __FILE__, __LINE__);
            MFMapperFree(mapper);
            delete netIF;
            ret = DBFault;
            goto Stop;
        }
        for (objID = 0; objID < mapper->ObjNum; ++objID) mapper->SampleIDs [objID] = DBFault;
        switch (mapperData->Type()) {
            case DBTypeVectorPoint: {
                DBVPointIF *pntIF = new DBVPointIF(mapperData);
                DBObjRecord *pntRec;
                mapper->Type = MFsamplePoint;
                mapper->SampleNum = pntIF->ItemNum();
                for (sampleID = 0;sampleID < mapper->SampleNum; ++sampleID) {
                    pntRec = pntIF->Item (sampleID);
                    coord = pntIF->Coordinate (pntRec);
                    if ((cellRec = netIF->Cell(coord)) != (DBObjRecord *) NULL) {
                        mapper->SampleIDs [cellRec->RowID ()] = pntRec->RowID ();
                    }
                }
            } break;
            case DBTypeGridDiscrete: {
                DBGridIF *gridIF = new DBGridIF(mapperData);
                DBObjRecord *gridRec;

                mapper->Type = MFsampleZone;
                mapper->SampleNum = (mapperData->Table(DBrNItems))->ItemNum ();
                for (objID = 0;objID < mapper->ObjNum; ++objID) {
                    cellRec = netIF->Cell(objID);
                    coord   = netIF->Center (cellRec);
                    if ((gridRec = gridIF->GridItem (coord)) != (DBObjRecord *) NULL) {
                        mapper->SampleIDs [objID] = gridRec->RowID ();
                    }
                }
            } break;
            default: {
                CMmsgPrint(CMmsgUsrError, "Mapper reading error in: %s", CMfileName(argv[0]));
                ret = DBFault;
                goto Stop;
            }
        }
        ret = MFMapperWrite(mapper, outFile);
    }
    Stop:
    delete domainData;
    delete mapperData;
    if (outFile != stdout) fclose(outFile);
    return (ret);
}
