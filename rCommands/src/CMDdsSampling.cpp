/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDdsSampling.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <cm.h>
#include <DB.hpp>
#include <MF.h>
#include <stdlib.h>
#include <string.h>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <datastream1> <datastream2> ... <datastreamN>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "  -D, --domainfile <filename>");
    CMmsgPrint(CMmsgInfo, "  -M, --mapper     <filename>");
    CMmsgPrint(CMmsgInfo, "  -o, --output     <filename>");
    CMmsgPrint(CMmsgInfo, "  -t,--title       [dataset title]");
    CMmsgPrint(CMmsgInfo, "  -u,--subject     [subject]");
    CMmsgPrint(CMmsgInfo, "  -d,--domain      [domain]");
    CMmsgPrint(CMmsgInfo, "  -v,--version     [version]");
    CMmsgPrint(CMmsgUsrError, "  -h,--help");
}

typedef struct MFMapperStats_s {
    int Count;
    double Area;
    double Mean;
    double Min;
    double Max;
    double StdDev;
} MFMapperStats_t, *MFMapperStats_p;

int main(int argc, char *argv[]) {
    bool compressed = false;
    int argPos = 0, argNum = argc, ret = CMfailed, itemSize, itemID, sampleID, maxCount;
    double val, maxVal = -HUGE_VAL;
    FILE *inFile;
    void *items = (void *) NULL;
    char *domainFileName = (char *) NULL, *mapperFileName = (char *) NULL, *outFileName = (char *) NULL;
    char *title  = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    DBDate date;
    MFDomain_p domainPTR = (MFDomain_p) NULL;
    MFMapper_p mapperPTR = (MFMapper_p) NULL;
    MFdsHeader_t header;
    MFMapperStats_p mapperStats;
    DBObjData  *data;
    DBObjTable *table;
    DBObjTableField *sampleIDFLD;
    DBObjTableField *dateFLD;
    DBObjTableField *zoneAreaFLD;
    DBObjTableField *valueFLD;
    DBObjTableField *meanValueFLD;
    DBObjTableField *minValueFLD;
    DBObjTableField *maxValueFLD;
    DBObjTableField *stdDevFLD;
    DBObjRecord *tblRec;

    if (argNum < 2) goto Help;

    for (argPos = 1; argPos < argNum;) {
        if (CMargTest(argv[argPos], "-D", "--domainfile")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing sampling item!");
                return (CMfailed);
            }
            domainFileName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-M", "--mapper")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing sampling item!");
                return (CMfailed);
            }
            mapperFileName = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-o", "--output")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing output file!");
                return (CMfailed);
            }
            outFileName = argv[argPos];
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
Help:   if (CMargTest(argv[argPos], "-h", "--help")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) < argPos) break;
            _CMDprintUsage (argv[0]);
            ret = CMsucceeded;
            goto Stop;
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }

    if (domainFileName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Missing domain file name!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if (mapperFileName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Missing mapper file name!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if ((inFile = fopen(domainFileName, "r")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Domain file [%s] opening error!",mapperFileName);
        return (CMfailed);
    }
    if ((domainPTR = MFDomainRead (inFile)) == (MFDomain_p) NULL) {
        CMmsgPrint(CMmsgUsrError, "Domain reading error!",mapperFileName);
        fclose(inFile);
        MFDomainFree (domainPTR);
        return (CMfailed);
    }
     if ((inFile = fopen(mapperFileName, "r")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Mapper file [%s] opening error!",mapperFileName);
        return (CMfailed);
    }
    if ((mapperPTR = MFMapperRead (inFile)) == (MFMapper_p) NULL) {
        CMmsgPrint(CMmsgUsrError, "Mapper reading error!",mapperFileName);
        fclose(inFile);
        MFDomainFree  (domainPTR);
        MFMapperFree (mapperPTR);
        return (CMfailed);
    }
    fclose (inFile);
    if (domainPTR->ObjNum != mapperPTR->ObjNum) {
        CMmsgPrint(CMmsgUsrError, "Domain and Mapper missmatch!");
        MFDomainFree  (domainPTR);
        MFMapperFree (mapperPTR);
        return (CMfailed);
    }
    if ((mapperStats = (MFMapperStats_p) calloc (mapperPTR->ObjNum,sizeof(MFMapperStats_t))) == (MFMapperStats_p) NULL) {
        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
        return (CMfailed);
    }

    if (title   == (char *) NULL) title   = (char *) "Model Statistics";
    if (subject == (char *) NULL) subject = (char *) "Statistics";
    if (domain  == (char *) NULL) domain  = (char *) "Unspecified Domain";
    if (version == (char *) NULL) version = (char *) "0.01pre";

    data = new DBObjData(title, DBTypeTable);
    data->Document(DBDocSubject,   subject);
    data->Document(DBDocGeoDomain, domain);
    data->Document(DBDocVersion,   version);
    table = data->Table(DBrNItems);
    switch (mapperPTR->Type) {
        case MFsamplePoint:
            table->AddField (sampleIDFLD  = new DBObjTableField("SampleID", DBTableFieldInt,   "%8d",    sizeof(DBInt)));
            table->AddField (dateFLD      = new DBObjTableField("Date",     DBTableFieldDate,  "%s",     sizeof(DBDate)));
            table->AddField (valueFLD     = new DBObjTableField("Value",    DBTableFieldFloat, "%10.1f", sizeof(DBFloat4)));
            break; 
        case MFsampleZone:
            table->AddField (sampleIDFLD  = new DBObjTableField("SampleID", DBTableFieldInt,   "%8d",    sizeof(DBInt)));
            table->AddField (dateFLD      = new DBObjTableField("Date",     DBTableFieldDate,  "%s",     sizeof(DBDate)));
            table->AddField (zoneAreaFLD  = new DBObjTableField("ZoneArea", DBTableFieldFloat, "%10.0f", sizeof(DBFloat4)));
            table->AddField (meanValueFLD = new DBObjTableField("Mean",     DBTableFieldFloat, "%10.3f", sizeof(DBFloat4)));
            table->AddField (minValueFLD  = new DBObjTableField("Minimum",  DBTableFieldFloat, "%10.3f", sizeof(DBFloat4)));
            table->AddField (maxValueFLD  = new DBObjTableField("Maximum",  DBTableFieldFloat, "%10.3f", sizeof(DBFloat4)));
            table->AddField (stdDevFLD    = new DBObjTableField("StdDev",   DBTableFieldFloat, "%10.3f", sizeof(DBFloat4)));
            break;
        default: break;
    }

    for (argPos = 0; argPos < argNum; ++argPos) {
        if (argNum > 1) {
            if (argPos == 0) continue;
            if ((argPos == 1) && (strcmp(argv[argPos], "-") == 0)) inFile = stdin;
            else {
                if (strncmp(CMfileExtension(argv[argPos]), "gz", 2) == 0) {
                    char pCommand[strlen(argv[argPos]) + 16];
                    sprintf(pCommand, "gunzip -c %s", argv[argPos]);
                    inFile = popen(pCommand, "r");
                    compressed = true;
                }
                else {
                    inFile = fopen(argv[argPos], "r");
                    compressed  = false;
                }
            }
        } else inFile = stdin;        
        if (inFile == (FILE *) NULL) {
            CMmsgPrint(CMmsgSysError, "Input file opening error in: %s %d", __FILE__, __LINE__);
            goto Stop;
        }

        while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
            if (header.ItemNum != mapperPTR->ObjNum) {
                CMmsgPrint(CMmsgUsrError, "Data stream [%d] and mapperPTR [%d] missmatch!", header.ItemNum, mapperPTR->ObjNum);
                goto Stop;
            }
            if (items == (void *) NULL) {
                itemSize = MFVarItemSize(header.Type);
                if ((items = (void *) calloc(header.ItemNum, itemSize)) == (void *) NULL) {
                    CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s:%d", __FILE__, __LINE__);
                    goto Stop;
                }
            }
            else {
                if (itemSize != MFVarItemSize(header.Type)) {
                    CMmsgPrint(CMmsgUsrError, "Inconsisten data stream sequence!");
                    goto Stop;
                }
            }
            if ((int) fread(items, itemSize, header.ItemNum, inFile) != header.ItemNum) {
                CMmsgPrint(CMmsgSysError, "Data stream reading error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
            switch (mapperPTR->Type) {
                case MFsamplePoint:
                    for (sampleID = 0; sampleID < mapperPTR->ObjNum; ++sampleID) {
                        mapperStats [sampleID].Count = 0;
                        mapperStats [sampleID].Mean  = valueFLD->FloatNoData ();
                    }
                    break; 
                case MFsampleZone:
                    for (sampleID = 0; sampleID < mapperPTR->ObjNum; ++sampleID) {
                        mapperStats [sampleID].Count  = 0;
                        mapperStats [sampleID].Area   =
                        mapperStats [sampleID].Mean   =
                        mapperStats [sampleID].StdDev = 0.0;
                        mapperStats [sampleID].Min    =  HUGE_VAL;
                        mapperStats [sampleID].Max    = -HUGE_VAL;
                    }
                    break;
                default: break;
            }
            maxCount = 0;
            date.Set (header.Date);
            for (itemID = 0; itemID < header.ItemNum; ++itemID) {
                sampleID = mapperPTR->SampleIDs[itemID];
                if (sampleID == DBFault) continue;
                if (sampleID >= mapperPTR->SampleNum) {
                    CMmsgPrint(CMmsgUsrError, "Inconsisten mapperPTR!");
                    goto Stop;
                }
                switch (header.Type) {
                    case MFByte:
                        if (((char *) items)[itemID]  == header.Missing.Int) continue;
                        val = (double) ((char *) items)[itemID];
                        break;
                    case MFShort:
                        if (header.Swap != 1) MFSwapHalfWord(((short *) items) + itemID);
                        if (((short *) items)[itemID] == header.Missing.Int) continue;
                        val = (double) ((short *) items)[itemID];
                        break;
                    case MFInt:
                        if (header.Swap != 1) MFSwapWord(((int *) items) + itemID);
                        if (((int *) items)[itemID]   == header.Missing.Int) continue;
                        val = (double) ((int *) items)[itemID];
                        break;
                    case MFFloat:
                        if (header.Swap != 1) MFSwapWord(((float *) items) + itemID);
                        if (CMmathEqualValues(((float *) items)[itemID],  header.Missing.Float)) continue;
                        val = (double) ((float *) items)[itemID];
                        break;
                    case MFDouble:
                        if (header.Swap != 1) MFSwapLongWord(((double *) items) + itemID);
                        if (CMmathEqualValues(((double *) items)[itemID], header.Missing.Float)) continue;
                        val = (double) ((double *) items)[itemID];
                        break;
                }
                mapperStats [sampleID].Count  += 1;
                switch (mapperPTR->Type) {
                    case MFsamplePoint: mapperStats [sampleID].Mean   = val; break;
                    case MFsampleZone:
                        mapperStats [sampleID].Area  += domainPTR->Objects[itemID].Area;
                        mapperStats [sampleID].Mean    += val * domainPTR->Objects[itemID].Area;
                        mapperStats [sampleID].Min      = val < mapperStats [sampleID].Min ? val : mapperStats [sampleID].Min;
                        mapperStats [sampleID].Max      = val > mapperStats [sampleID].Max ? val : mapperStats [sampleID].Max;
                        mapperStats [sampleID].StdDev  += val * val * domainPTR->Objects[itemID].Area;
                        break;
                    default: break;
                }
                maxCount = mapperStats [itemID].Count > maxCount ? mapperStats [itemID].Count : maxCount;
            }
           
            switch (mapperPTR->Type) {
                case MFsamplePoint:
                    if (maxCount > 1) {
                        CMmsgPrint(CMmsgUsrError, "Point mapperPTR have more than one point with the same ID!");
                        goto Stop;
                    }
                    for (sampleID = 0;sampleID < mapperPTR->SampleNum; ++sampleID) {
                        tblRec = table->Add ();
                        sampleIDFLD->Int (tblRec,sampleID + 1);
                        dateFLD->Date    (tblRec,date);
                        valueFLD->Float  (tblRec,mapperStats [sampleID].Mean);
                        maxVal = mapperStats [sampleID].Mean > maxVal ? mapperStats [sampleID].Mean : maxVal;
                    }
                    break;
                case MFsampleZone:
                    date.Set (header.Date);
                    for (sampleID = 0;sampleID < mapperPTR->SampleNum; ++sampleID) {
                        tblRec = table->Add ();
                        sampleIDFLD->Int (tblRec,sampleID + 1);
                        dateFLD->Date    (tblRec,date);
                        if (mapperStats [sampleID].Area > 0.0) {
                            mapperStats [sampleID].Mean   = mapperStats [sampleID].Mean   / mapperStats [sampleID].Area;
                            mapperStats [sampleID].StdDev = mapperStats [sampleID].StdDev / mapperStats [sampleID].Area - mapperStats [sampleID].Mean * mapperStats [sampleID].Mean;
                            mapperStats [sampleID].StdDev = mapperStats [sampleID].StdDev > 0.0 ? sqrt (mapperStats [sampleID].StdDev) : 0.0;
                            zoneAreaFLD->Float  (tblRec, mapperStats [sampleID].Area);
                            meanValueFLD->Float (tblRec, mapperStats [sampleID].Mean);
                            minValueFLD->Float  (tblRec, mapperStats [sampleID].Min);
                            maxValueFLD->Float  (tblRec, mapperStats [sampleID].Max);
                            stdDevFLD->Float    (tblRec, mapperStats [sampleID].StdDev);
                            maxVal = mapperStats [sampleID].Mean > maxVal ? mapperStats [sampleID].Mean : maxVal;
                        }
                        else {
                            zoneAreaFLD->Float  (tblRec, 0.0);
                            meanValueFLD->Float (tblRec, meanValueFLD->FloatNoData ());
                            minValueFLD->Float  (tblRec, minValueFLD->FloatNoData ());
                            maxValueFLD->Float  (tblRec, maxValueFLD->FloatNoData ());
                            stdDevFLD->Float    (tblRec, stdDevFLD->FloatNoData ());
                        }
                    }
                    break;
                default: break;
            }
        }
        if (ferror (inFile) != 0) {
            CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
            ret = CMfailed;
        }
        if (compressed) pclose (inFile); else fclose(inFile);
        inFile = stdin;
    }
    switch (mapperPTR->Type) {
        case MFsamplePoint:
                valueFLD->Format (DBMathFloatAutoFormat (maxVal));
            break;
        case MFsampleZone:
            meanValueFLD->Format (DBMathFloatAutoFormat (maxVal));
             minValueFLD->Format (DBMathFloatAutoFormat (maxVal));
             maxValueFLD->Format (DBMathFloatAutoFormat (maxVal));
               stdDevFLD->Format (DBMathFloatAutoFormat (maxVal));
            break;
        default: break;
    }
Stop:
    if (domainPTR != (MFDomain_p) NULL) MFDomainFree (domainPTR);
    if (mapperPTR != (MFMapper_p) NULL) MFMapperFree (mapperPTR);
    if (outFileName != (char *) NULL) data->Write (outFileName); else data->Write(stdout);
    delete data;
    if ((inFile != (FILE *) NULL) && (inFile != stdin)) { if (compressed) pclose (inFile); else fclose (inFile); }
    return (ret);
}
