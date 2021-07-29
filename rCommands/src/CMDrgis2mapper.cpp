/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDrgis2domain.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <DBif.hpp>
#include <MF.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <sampling data> <output sampler>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -d,--domain");
    CMmsgPrint(CMmsgInfo, "     -h,--help");
}

int main(int argc, char *argv[]) {
    FILE *outFile;
    DBInt argPos, argNum = argc, ret;
    int objID, sampleID;
    char *domain = (char *) NULL;
    MFSampler_p sampler = (MFSampler_p) NULL;
    DBCoordinate coord;
    DBObjData *domainData, *samplerData;
    DBNetworkIF *netIF;
    DBObjRecord *cellRec;

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

    samplerData = new DBObjData();
    ret = (argNum > 1) && (strcmp(argv[1], "-") != 0) ? samplerData->Read(argv[1]) : samplerData->Read(stdin);
    if (ret == DBFault) {
        CMmsgPrint(CMmsgUsrError, "Sampler reading error in: %s", CMfileName(argv[0]));
        _CMDprintUsage (argv[0]);
        delete domainData;
        if (outFile != stdout) fclose (outFile);
        return(DBFault);
    }

    if ((sampler = (MFSampler_p) calloc (1, sizeof(MFSampler_t))) != (MFSampler_p) NULL) {
        netIF = new DBNetworkIF (domainData);
        sampler->Swap = 1;
        sampler->ObjNum = netIF->CellNum ();
        if ((sampler->SampleIDs = (int *) calloc (sampler->ObjNum, sizeof (int))) == (int *) NULL) {
            CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: %s %d", __FILE__, __LINE__);
            MFSamplerFree(sampler);
            delete netIF;
            ret = DBFault;
            goto Stop;
        }
        for (objID = 0; objID < sampler->ObjNum; ++objID) sampler->SampleIDs [objID] = DBFault;
        switch (samplerData->Type()) {
            case DBTypeVectorPoint: {
                DBVPointIF *pntIF = new DBVPointIF(samplerData);
                DBObjRecord *pntRec;
                sampler->Type = MFsamplePoint;
                sampler->SampleNum = pntIF->ItemNum();
                for (sampleID = 0;sampleID < sampler->SampleNum; ++sampleID) {
                    pntRec = pntIF->Item (sampleID);
                    coord = pntIF->Coordinate (pntRec);
                    if ((cellRec = netIF->Cell(coord)) != (DBObjRecord *) NULL) {
                        sampler->SampleIDs [cellRec->RowID ()] = pntRec->RowID ();
                    }
                }
            } break;
            case DBTypeGridDiscrete: {
                DBGridIF *gridIF = new DBGridIF(samplerData);
                DBObjRecord *gridRec;

                sampler->Type = MFsampleZone;
                sampler->SampleNum = (samplerData->Table(DBrNItems))->ItemNum ();
                for (objID = 0;objID < sampler->ObjNum; ++objID) {
                    cellRec = netIF->Cell(objID);
                    coord   = netIF->Center (cellRec);
                    if ((gridRec = gridIF->GridItem (coord)) != (DBObjRecord *) NULL) {
                        sampler->SampleIDs [objID] = gridRec->RowID ();
                    }
                }
            } break;
            default: {
                CMmsgPrint(CMmsgUsrError, "Sampler reading error in: %s", CMfileName(argv[0]));
                ret = DBFault;
                goto Stop;
            }
        }
        ret = MFSamplerWrite(sampler, outFile);
    }
    Stop:
    delete domainData;
    delete samplerData;
    if (outFile != stdout) fclose(outFile);
    return (ret);
}
