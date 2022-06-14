/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

CMDdsStorage.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <string.h>
#include <stdlib.h>

class CMDgrdStorage {
    private:
        size_t ItemNum, RecordNum, SrcType, TrgType;
        void  *TrgBuffer;
        void  *SrcBuffer;
        float *CumulSum;
        float *MaxSum;
        MFdsHeader_p HeaderPTR = (MFdsHeader_p) NULL;
        MFdsHeader_t Header;
    public:
        int Initialize (FILE *file) {
            size_t itemID, itemSize, ret = CMfailed;

            TrgBuffer  = SrcBuffer = (void *)  NULL;
            CumulSum   = MaxSum    = (float *) NULL;
            RecordNum  = 0;

            while (MFdsHeaderRead (&Header, file) == CMsucceeded) {
                if ((HeaderPTR = (MFdsHeader_p) realloc (HeaderPTR, (RecordNum + 1) * sizeof (MFdsHeader_t))) == (MFdsHeader_p) NULL) {
                    CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                if ((memcpy (HeaderPTR + RecordNum, &Header, sizeof (MFdsHeader_t))) != HeaderPTR + RecordNum) {
                    CMmsgPrint(CMmsgSysError, "Memory copying error in: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                if (RecordNum == 0) {
                    ItemNum  = HeaderPTR[RecordNum].ItemNum;
                    TrgType  = HeaderPTR[RecordNum].Type;
                    switch (TrgType) {
                        case MFByte:   itemSize = sizeof (char);   break;
                        case MFShort:  itemSize = sizeof (short);  break;
                        case MFInt:    itemSize = sizeof (int);    break;
                        case MFFloat:  itemSize = sizeof (float);  break;
                        case MFDouble: itemSize = sizeof (double); break;
                    }
                    if ((CumulSum = (float *) calloc (ItemNum,sizeof (float))) == (float *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    if ((MaxSum   = (float *) calloc (ItemNum,sizeof (float))) == (float *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        goto Stop;
                    }
                    for (itemID = 0; itemID < ItemNum; ++itemID) CumulSum[itemID] = MaxSum[itemID] = 0.0;
                } else {
                    if ((ItemNum != HeaderPTR[RecordNum].ItemNum) || (TrgType != HeaderPTR[RecordNum].Type)) {
                        CMmsgPrint(CMmsgAppError, "Inconsistent target data stream: %s %d", __FILE__, __LINE__);
                        goto Stop;
                    }
                }
                if ((TrgBuffer = realloc (TrgBuffer, (RecordNum + 1) * ItemNum * itemSize)) == (void *) NULL) {
                    CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                if ((fread ((char *) TrgBuffer + (RecordNum) * ItemNum * itemSize, itemSize, ItemNum, file)) != ItemNum) {
                    CMmsgPrint(CMmsgAppError, "Target file reading error: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                RecordNum++;
            }
            if (RecordNum == 0) {
                CMmsgPrint(CMmsgSysError, "Missing target records: %s %d", __FILE__, __LINE__);
                goto Stop;
            }
            else ret = CMsucceeded;
Stop:       return (ret);
        }

        int Run (FILE *file, bool deficit) {
            size_t itemID, itemSize, recordID = 0, ret = CMfailed;
            int   intVal;
            float floatVal;
            float target, source;

            while (MFdsHeaderRead (&Header, file) == CMsucceeded) {
                if (Header.ItemNum != ItemNum) {
                    CMmsgPrint(CMmsgAppError, "Inconsistent target and source data streams: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                if (SrcBuffer != (void *) NULL) {
                    if (SrcType != Header.Type) {
                        CMmsgPrint(CMmsgAppError, "Inconsistent source data stream: %s %d", __FILE__, __LINE__);
                        goto Stop;
                    }
                } else {
                    SrcType = Header.Type;
                    switch (SrcType) {
                        case MFByte:   itemSize = sizeof (char);   break;
                        case MFShort:  itemSize = sizeof (short);  break;
                        case MFInt:    itemSize = sizeof (int);    break;
                        case MFFloat:  itemSize = sizeof (float);  break;
                        case MFDouble: itemSize = sizeof (double); break;
                    }
                    if ((SrcBuffer = calloc (itemSize, ItemNum)) == (void *) NULL) {
                        CMmsgPrint(CMmsgAppError, "Memory allocation error: %s %d", __FILE__, __LINE__);
                        goto Stop;
                    }
                }
                if ((fread ((char *) SrcBuffer, itemSize, ItemNum, file)) != ItemNum) {
                    CMmsgPrint(CMmsgAppError, "Source file reading error: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                if (RecordNum > 1) {
                    for ( recordID = recordID; recordID < RecordNum; recordID++)
                        if (strncmp (HeaderPTR[recordID].Date + 5, Header.Date + 5, strlen (HeaderPTR[recordID].Date + 5)) != 0) break;
                }
                if (recordID >= RecordNum) {
                    CMmsgPrint(CMmsgAppError, "Missmatching source and target layernames: %s %d", __FILE__, __LINE__);
                    goto Stop;  
                }
                for (itemID = 0; itemID < ItemNum; ++itemID) {
                    switch (TrgType) {
                        case MFByte:
                            intVal = (int) ((char *)   TrgBuffer) [recordID * ItemNum + itemID];
                            if (intVal == HeaderPTR [recordID].Missing.Int) continue;
                            else target = (float) intVal;
                            break;
                        case MFShort:
                            intVal = (int) ((short *)  TrgBuffer) [recordID * ItemNum + itemID];
                            if (intVal == HeaderPTR [recordID].Missing.Int) continue;
                            else target = (float) intVal;
                            break;
                        case MFInt:
                            intVal = (int) ((int *)    TrgBuffer) [recordID * ItemNum + itemID];
                            if (intVal == HeaderPTR [recordID].Missing.Int) continue;
                            else target = (float) intVal;
                            break;
                        case MFFloat:
                            floatVal = (float) ((float *)  TrgBuffer) [recordID * ItemNum + itemID];
                            if (CMmathEqualValues (floatVal, HeaderPTR [recordID].Missing.Float)) continue;
                            else target = (float) floatVal;
                            break;
                        case MFDouble:
                            floatVal = (float) ((double *) TrgBuffer) [recordID * ItemNum + itemID];
                            if (CMmathEqualValues (floatVal, HeaderPTR [recordID].Missing.Float)) continue;
                            else target = (float) floatVal;
                            break;
                    }
                    switch (SrcType) {
                        case MFByte:
                            intVal = (int) ((char *)   SrcBuffer) [itemID];
                            if (intVal == Header.Missing.Int) continue;
                            else source = (float) intVal;
                            break;
                        case MFShort:
                            intVal = (int) ((short *)  SrcBuffer) [itemID];
                            if (intVal == Header.Missing.Int) continue;
                            else source = (float) intVal;
                            break;
                        case MFInt:
                            intVal = (int) ((int *)    SrcBuffer) [itemID];
                            if (intVal == Header.Missing.Int) continue;
                            else source = (float) intVal;
                            break;
                        case MFFloat:
                            floatVal = (float) ((float *)  SrcBuffer) [itemID];
                            if (CMmathEqualValues (floatVal, Header.Missing.Float)) continue;
                            else source = (float) floatVal;
                            break;
                        case MFDouble:
                            floatVal = (float) ((double *) SrcBuffer) [itemID];
                            if (CMmathEqualValues (floatVal, Header.Missing.Float)) continue;
                            else source = (float) floatVal;
                            break;
                    }
                    CumulSum [itemID] += deficit ? target - source : source - target;
                    if (CumulSum [itemID] < 0.0) CumulSum [itemID] = 0.0;
                    if (CumulSum [itemID] > MaxSum [itemID]) MaxSum [itemID] = CumulSum [itemID];
                }
            }
            ret = CMsucceeded;
Stop:       return (ret);
        }
        int Finalize (FILE *file,int ret) {
            if (ret == CMsucceeded) {
                ret = CMfailed;
                Header.Type = MFFloat;
                strcpy (Header.Date,"XXXX");
                if (MFdsHeaderWrite (&Header,file) == CMfailed) {
                    CMmsgPrint(CMmsgAppError, "Output file writing error: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }               
                if (fwrite(MaxSum,sizeof(float),ItemNum,file) != ItemNum) {
                    CMmsgPrint(CMmsgAppError, "Output file writing error: %s %d", __FILE__, __LINE__);
                    goto Stop;
                }
                ret = CMsucceeded;
            }
Stop:       if (HeaderPTR != (MFdsHeader_p) NULL) free (HeaderPTR);
            if (TrgBuffer != (void *)       NULL) free (TrgBuffer);
            if (SrcBuffer != (void *)       NULL) free (SrcBuffer);
            if (CumulSum  != (float *)      NULL) free (CumulSum);
            if (MaxSum    != (float *)      NULL) free (MaxSum);
            return (ret);
        }
};

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s [options] <data stream 0> <input datastream 1> ... <data stream N>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo,     "     -T, --target    [target datastream] => defaults to stdin");
    CMmsgPrint(CMmsgUsrError, "     -m, --mode      [deficit|surplus]");
    CMmsgPrint(CMmsgInfo,     "     -o, --output    [data stream]       => defaults to stdout");
    CMmsgPrint(CMmsgInfo,     "     -h, --help");
}

int main(int argc, char *argv[]) {
    int argPos, argNum = argc, ret, deficit = true, pipe = false;
    char *target = (char *) NULL, *output;
    FILE *file = (FILE *) NULL;
    CMDgrdStorage grdStorage;

    if (argNum < 2) goto Help;
 
    for (argPos = 1; argPos < argNum;) {
        if (CMargTest (argv[argPos], "-T", "--target")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing target grid!");
                return (CMfailed);
            }
            target = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest(argv[argPos], "-m", "--mode")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missging mode option!");
                return (CMfailed);
            }
            else {
                const char *options[] = {"deficit", "surplus", (char *) NULL};
                int codes[] = { true, false }, code;

                if ((code = CMoptLookup(options, argv[argPos], false)) == CMfailed) {
                    CMmsgPrint(CMmsgWarning, "Ignoring illformed step option [%s]!", argv[argPos]);
                    return (CMfailed);
                }
                else deficit = codes[code];
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
        if (CMargTest (argv[argPos], "-o", "--output")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing merge grid!");
                return (CMfailed);
            }
            output = argv[argPos];
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }
Help:   if (CMargTest (argv[argPos], "-h", "--help")) {
            _CMDprintUsage(argv[0]);
            return (CMsucceeded);
        }
        if ((argv[argPos][0] == '-') && (strlen(argv[argPos]) > 1)) {
            CMmsgPrint(CMmsgUsrError, "Unknown option: %s!", argv[argPos]);
            return (CMfailed);
        }
        argPos++;
    }
    if (argNum < 2)              { CMmsgPrint(CMmsgUsrError, "Missing time series data!"); return (CMfailed); }

    if (target == (char *) NULL) file = stdin;
    else {
        if (strcmp (target + strlen(target) - 3,".gz") == 0) {
            char command [strlen(target) + 12];
            sprintf (command,"gunzip -c %s",target);
            if ((file = popen (command,"r")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Target file opening error: %s!", argv[argPos]);
                return (CMfailed);    
            } else pipe = true;
        } else {
            if ((file = fopen (target,"r")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Target file opening error: %s!", argv[argPos]);
                return (CMfailed);
            } else pipe = false;
        }
    }
    if (grdStorage.Initialize (file) == CMfailed) goto Stop;
    if (file != stdin) { if (pipe) pclose (file); else fclose (file); }
    for (argPos = 2; argPos < argNum; ++argPos) {
        if (strcmp (argv[argPos] + strlen(target) - 3,".gz") == 0) {
            char command [strlen(target) + 12];
            sprintf (command,"gunzip -c %s",target);
            if ((file = popen (command,"r")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Source file opening error: %s!", argv[argPos]);
                goto Stop;    
            } else pipe = true;
        } else {
            if ((file = fopen (argv[argPos],"r")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Source file opening error: %s!", argv[argPos]);
                goto Stop;
            } else pipe = false;
        }
        if (grdStorage.Run (file,deficit) == CMfailed) goto Stop;
    }
    if (output == (char *) NULL) file = stdout;
    else {
        if (strcmp (output + strlen(output) - 3,".gz") == 0) {
            char command [strlen(output) + 12];
            sprintf (command,"gunzip -c %s",output);
            if ((file = popen (command,"w")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Output file opening error: %s!", argv[argPos]);
                goto Stop; 
            } else pipe = true;
        } else {
            if ((file = fopen (output,"w")) == (FILE *) NULL) {
                CMmsgPrint(CMmsgUsrError, "Output fiel opening error: %s!", argv[argPos]);
                goto Stop;
            } else pipe = false;
        }
    }
    ret = CMsucceeded;
Stop:
    ret = grdStorage.Finalize (file, ret);
    if ((file != (FILE *) NULL) && (file != stdin) && (file != stdout)) { if (pipe) pclose (file); else fclose (file); }
    return (ret);
}