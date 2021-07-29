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
    CMmsgPrint(CMmsgInfo, "%s [options] <datastream 1> <datastream 2> ... <datastream N>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "  -D, --domainfile [filename]");
    CMmsgPrint(CMmsgInfo, "  -S, --sampler    [filename]");
    CMmsgPrint(CMmsgInfo, "  -o, --output     [filename]");
    CMmsgPrint(CMmsgInfo, "  -t,--title       [dataset title]");
    CMmsgPrint(CMmsgInfo, "  -u,--subject     [subject]");
    CMmsgPrint(CMmsgInfo, "  -d,--domain      [domain]");
    CMmsgPrint(CMmsgInfo, "  -v,--version     [version]");
    CMmsgPrint(CMmsgUsrError, "  -h,--help");
}

typedef struct MFSamplerStats_s {
    int Count;
    double Area;
    double Mean;
    double Min;
    double Max;
    double StdDev;
} MFSamplerStats_t, *MFSamplerStats_p;

int main(int argc, char *argv[]) {
    bool compressed = false;
    int argPos = 0, argNum = argc, ret = CMfailed, itemSize, itemID, sampleID, maxCount;
    double val, maxVal = -HUGE_VAL;
    FILE *inFile;
    void *items = (void *) NULL;
    char *domainFileName = (char *) NULL, *samplerFileName = (char *) NULL, *outFileName = (char *) NULL;
    char *title  = (char *) NULL, *subject = (char *) NULL;
    char *domain = (char *) NULL, *version = (char *) NULL;
    DBDate date;
    MFDomain_p  domainPTR;
    MFSampler_p samplerPTR;
    MFdsHeader_t header;
    MFSamplerStats_p samplerStats;
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
        if (CMargTest(argv[argPos], "-S", "--sampler")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing sampling item!");
                return (CMfailed);
            }
            samplerFileName = argv[argPos];
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
    if (argNum > 3) {
        CMmsgPrint(CMmsgUsrError, "Extra arguments!");
        _CMDprintUsage (argv[0]);
        return(CMfailed);
    }

    if (domainFileName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Missing domain file name!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if (samplerFileName == (char *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Missing sampler file name!");
        _CMDprintUsage (argv[0]);
        return (CMfailed);
    }
    if ((inFile = fopen(domainFileName, "r")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Domain file [%s] opening error!",samplerFileName);
        return (CMfailed);
    }
    if ((domainPTR = MFDomainRead (inFile)) == (MFDomain_p) NULL) {
        CMmsgPrint(CMmsgUsrError, "Domain reading error!",samplerFileName);
        fclose(inFile);
        MFDomainFree (domainPTR);
        return (CMfailed);
    }
     if ((inFile = fopen(samplerFileName, "r")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgUsrError, "Sampler file [%s] opening error!",samplerFileName);
        return (CMfailed);
    }
    if ((samplerPTR = MFSamplerRead (inFile)) == (MFSampler_p) NULL) {
        CMmsgPrint(CMmsgUsrError, "Sampler reading error!",samplerFileName);
        fclose(inFile);
        MFDomainFree  (domainPTR);
        MFSamplerFree (samplerPTR);
        return (CMfailed);
    }
    fclose (inFile);
    if (domainPTR->ObjNum != samplerPTR->ObjNum) {
        CMmsgPrint(CMmsgUsrError, "Domain and Sampler missmatch!");
        MFDomainFree  (domainPTR);
        MFSamplerFree (samplerPTR);
        return (CMfailed);
    }
    if ((samplerStats = (MFSamplerStats_p) calloc (samplerPTR->ObjNum,sizeof(MFSamplerStats_t))) == (MFSamplerStats_p) NULL) {
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
    switch (samplerPTR->Type) {
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
            if (header.ItemNum != samplerPTR->ObjNum) {
                CMmsgPrint(CMmsgUsrError, "Data stream [%d] and samplerPTR [%d] missmatch!", header.ItemNum, samplerPTR->ObjNum);
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
            switch (samplerPTR->Type) {
                case MFsamplePoint:
                    for (sampleID = 0; sampleID < samplerPTR->ObjNum; ++sampleID) {
                        samplerStats [sampleID].Count = 0;
                        samplerStats [sampleID].Mean  = valueFLD->FloatNoData ();
                    }
                    break; 
                case MFsampleZone:
                    for (sampleID = 0; sampleID < samplerPTR->ObjNum; ++sampleID) {
                        samplerStats [sampleID].Count  = 0;
                        samplerStats [sampleID].Area   =
                        samplerStats [sampleID].Mean   =
                        samplerStats [sampleID].StdDev = 0.0;
                        samplerStats [sampleID].Min    =  HUGE_VAL;
                        samplerStats [sampleID].Max    = -HUGE_VAL;
                    }
                    break;
                default: break;
            }
            maxCount = 0;
            date.Set (header.Date);
            for (itemID = 0; itemID < header.ItemNum; ++itemID) {
                sampleID = samplerPTR->SampleIDs[itemID];
                if (sampleID == DBFault) continue;
                if (sampleID >= samplerPTR->SampleNum) {
                    CMmsgPrint(CMmsgUsrError, "Inconsisten samplerPTR!");
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
                samplerStats [sampleID].Count  += 1;
                switch (samplerPTR->Type) {
                    case MFsamplePoint: samplerStats [sampleID].Mean   = val; break;
                    case MFsampleZone:
                        samplerStats [sampleID].Area  += domainPTR->Objects[itemID].Area;
                        samplerStats [sampleID].Mean    += val * domainPTR->Objects[itemID].Area;
                        samplerStats [sampleID].Min      = val < samplerStats [sampleID].Min ? val : samplerStats [sampleID].Min;
                        samplerStats [sampleID].Max      = val > samplerStats [sampleID].Max ? val : samplerStats [sampleID].Max;
                        samplerStats [sampleID].StdDev  += val * val * domainPTR->Objects[itemID].Area;
                        break;
                    default: break;
                }
                maxCount = samplerStats [itemID].Count > maxCount ? samplerStats [itemID].Count : maxCount;
            }
           
            switch (samplerPTR->Type) {
                case MFsamplePoint:
                    if (maxCount > 1) {
                        CMmsgPrint(CMmsgUsrError, "Point samplerPTR have more than one point with the same ID!");
                        goto Stop;
                    }
                    for (sampleID = 0;sampleID < samplerPTR->SampleNum; ++sampleID) {
                        tblRec = table->Add ();
                        sampleIDFLD->Int (tblRec,sampleID + 1);
                        dateFLD->Date    (tblRec,date);
                        valueFLD->Float  (tblRec,samplerStats [sampleID].Mean);
                        maxVal = samplerStats [sampleID].Mean > maxVal ? samplerStats [sampleID].Mean : maxVal;
                    }
                    break;
                case MFsampleZone:
                    date.Set (header.Date);
                    for (sampleID = 0;sampleID < samplerPTR->SampleNum; ++sampleID) {
                        tblRec = table->Add ();
                        sampleIDFLD->Int (tblRec,sampleID + 1);
                        dateFLD->Date    (tblRec,date);
                        if (samplerStats [sampleID].Area > 0.0) {
                            samplerStats [sampleID].Mean   = samplerStats [sampleID].Mean   / samplerStats [sampleID].Area;
                            samplerStats [sampleID].StdDev = samplerStats [sampleID].StdDev / samplerStats [sampleID].Area - samplerStats [sampleID].Mean * samplerStats [sampleID].Mean;
                            samplerStats [sampleID].StdDev = samplerStats [sampleID].StdDev > 0.0 ? sqrt (samplerStats [sampleID].StdDev) : 0.0;
                            zoneAreaFLD->Float  (tblRec, samplerStats [sampleID].Area);
                            meanValueFLD->Float (tblRec, samplerStats [sampleID].Mean);
                            minValueFLD->Float  (tblRec, samplerStats [sampleID].Min);
                            maxValueFLD->Float  (tblRec, samplerStats [sampleID].Max);
                            stdDevFLD->Float    (tblRec, samplerStats [sampleID].StdDev);
                            maxVal = samplerStats [sampleID].Mean > maxVal ? samplerStats [sampleID].Mean : maxVal;
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
    switch (samplerPTR->Type) {
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
    MFDomainFree  (domainPTR);
    MFSamplerFree (samplerPTR);
    if (outFileName != (char *) NULL) data->Write (outFileName); else data->Write(stdout);
    delete data;
    if (inFile != stdin) { if (compressed) pclose (inFile); else fclose (inFile); }
    return (ret);
}
