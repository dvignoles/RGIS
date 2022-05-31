/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

MFModel.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <cm.h>
#include <MF.h>
#include <time.h>

static MFDomain_p _MFDomain     = (MFDomain_p) NULL;
static MFFunction *_MFFunctions = (MFFunction *) NULL;
static int _MFFunctionNum = 0;

int MFModelAddFunction (MFFunction func) {

	if ((_MFFunctions = (MFFunction *) realloc (_MFFunctions, (_MFFunctionNum + 1) * sizeof (MFFunction))) == (MFFunction *) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
		return (CMfailed);
	}
	_MFFunctions [_MFFunctionNum] = func;
	_MFFunctionNum++;
	return (CMsucceeded);
}

float MFModelGetXCoord (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].XCoord);
}

float MFModelGetYCoord (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].YCoord);
}

float MFModelGetLongitude (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Lon);
}

float MFModelGetLatitude (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Lat);
}

float MFModelGetArea (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Area * 1000000.0);
}

float MFModelGetLength (int itemID) {
	if ((itemID < 0) || (itemID >= _MFDomain->ObjNum)) return (0.0);
	return (_MFDomain->Objects [itemID].Length * 1000.0);
}

int MFModelGetDownLink (int itemID,size_t linkNum) {
	if (itemID < 0) return (CMfailed);
	if (_MFDomain != (MFDomain_p) NULL) return (CMfailed);
	if (_MFDomain->ObjNum <= itemID)  return (CMfailed);
	if (_MFDomain->Objects [itemID].DLinkNum <= linkNum) return (CMfailed);

	return (_MFDomain->Objects [itemID].DLinks [linkNum]);
}

float MFModelGet_dt () { return (86400.0); }

typedef struct varEntry_s {
	bool InUse;
	char *Name;
	char *Path;
} varEntry_t, *varEntry_p;

static varEntry_p _MFModelVarEntryNew (varEntry_p varEntries, int entryID, const char *name, const char *path) {
	varEntries = (varEntry_p) realloc (varEntries,sizeof (varEntry_t)  * (entryID + 1));
	if (varEntries == (varEntry_p) NULL) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
		return ((varEntry_p) NULL);
	}
	varEntries [entryID].Name  = (char *) malloc (strlen (name) + 1);
	varEntries [entryID].Path  = (char *) malloc (strlen (path) + 1);
	if ((varEntries [entryID].Name == (char *) NULL) || (varEntries [entryID].Path == (char *) NULL)) {
		CMmsgPrint (CMmsgSysError,"Memory Allocation Error in: %s:%d",__FILE__,__LINE__);
        return ((varEntry_p) NULL);
	}
	strcpy (varEntries [entryID].Name, name);
	strcpy (varEntries [entryID].Path, path);
	varEntries [entryID].Name  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (varEntries [entryID].Name)));
	varEntries [entryID].Path  = CMbufStripDQuotes (CMbufStripSQuotes (CMbufTrim (varEntries [entryID].Path)));
	varEntries [entryID].InUse = false;
	return (varEntries);
}

static varEntry_p _MFModelVarEntryFind (varEntry_p varEntries, int varEntryNum, const char *name) {
	int i;

	for (i = 0; i < varEntryNum; ++i) if (strcmp (varEntries [i].Name,name) == 0) return (varEntries + i);
	return ((varEntry_p) NULL);
}

static void _MFModelVarEntriesFree (varEntry_p varEntries, int varEntryNum) {
	int i;
	if (varEntryNum > 0) {
		for (i = 0; i < varEntryNum; ++i) {
			free (varEntries [i].Name);
			if (varEntries   [i].InUse == false) free (varEntries [i].Path);
		}
		free (varEntries);
	}
}

static void _MFModelVarPrintOut (const char *label) {
    int varID;
    MFVariable_p var;

    CMmsgPrint (CMmsgInfo, "ID  %10s %30s[%10s] %6s %5s NStep %3s %4s %8s Output", label, "Variable","Unit","Type", "TStep", "Set", "Flux", "Initial");
    for (var = MFVarGetByID (varID = 1);var != (MFVariable_p) NULL;var = MFVarGetByID (++varID))
        if ((strncmp (var->Name,"__",2) != 0) || var->Initial)
            CMmsgPrint (CMmsgInfo, "%3i %10s %30s[%10s] %6s %5s %5d %3s %4s %8s %6s",
                        varID,var->InDate,var->Name,var->Unit,MFVarTypeString (var->Type),MFDateTimeStepString (var->TStep),var->NStep,
                        CMyesNoString (var->Set),CMyesNoString (var->Flux),CMyesNoString (var->Initial), CMyesNoString (var->OutputPath != (char *) NULL));
}

static CMthreadTeam_p _MFModelParse (int argc, char *argv [],int argNum, int (*mainDefFunc) (), char **domainFile, char **startDate, char **endDate, bool *testOnly) {
	bool resolved = true;
	int argPos, ret = CMsucceeded, help = false, procNum;
	int i, varID;
	varEntry_p inputVars  = (varEntry_p) NULL;
	varEntry_p outputVars = (varEntry_p) NULL;
	varEntry_p stateVars  = (varEntry_p) NULL;
	varEntry_p varEntry;
	int inputVarNum = 0, outputVarNum = 0, stateVarNum = 0;
	MFVariable_p var;
    bool _MFOptionTestInUse ();

	*testOnly = false;
	procNum = CMthreadProcessorNum ();
    for (argPos = 1;argPos < argNum;) {
		if (CMargTest (argv [argPos],"-i","--input")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing input argument!");
				return ((CMthreadTeam_p) NULL);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint  (CMmsgUsrError,"Illformed input variable [%s] in: %s:%d",argv [argPos],__FILE__,__LINE__);
				return ((CMthreadTeam_p) NULL);
			}
			argv [argPos][i] = '\0';
            inputVars = _MFModelVarEntryNew (inputVars, inputVarNum, argv [argPos],argv [argPos] + i + 1);
            if (inputVars == (varEntry_p) NULL) return ((CMthreadTeam_p) NULL); else inputVarNum++;
            if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-o","--output")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing _MFModelOutput argument!\n");
				return ((CMthreadTeam_p) NULL);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint (CMmsgUsrError,"Illformed _MFModelOutput variable [%s]!",argv [argPos]);
				return ((CMthreadTeam_p) NULL);
			}
			argv [argPos][i] = '\0';
			outputVars = _MFModelVarEntryNew (outputVars, outputVarNum, argv [argPos],argv [argPos] + i + 1);
			if (outputVars == (varEntry_p) NULL) return ((CMthreadTeam_p) NULL); else outputVarNum++;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
	 	}
		if (CMargTest (argv [argPos],"-t","--state")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing _MFModelOutput argument!\n");
				return ((CMthreadTeam_p) NULL);
			}
			for (i = 0;i < (int) strlen (argv[argPos]);++i) if (argv [argPos][i] == '=') break;
			if (i == (int) strlen (argv [argPos])) {
				CMmsgPrint (CMmsgUsrError,"Illformed _MFModelOutput variable [%s]!",argv [argPos]);
				return ((CMthreadTeam_p) NULL);
			}
			argv [argPos][i] = '\0';
			stateVars = _MFModelVarEntryNew (stateVars, stateVarNum, argv [argPos],argv [argPos] + i + 1);
			if (stateVars == (varEntry_p) NULL) return ((CMthreadTeam_p) NULL); else stateVarNum++;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
	 	}
		if (CMargTest (argv [argPos],"-s","--start")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing start time!");
				return ((CMthreadTeam_p) NULL);
			}
			*startDate = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-n","--end")) {
			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing end time!");
				return ((CMthreadTeam_p) NULL);
			}
			*endDate = argv [argPos];
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
		if (CMargTest (argv [argPos],"-T","--testonly")) {
			*testOnly = true;
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
            continue;
		}
		if (CMargTest (argv [argPos],"-m","--message")) {
			const char *types [] = { "sys_error", "app_error", "usr_error", "debug", "warning", "info", (char *) NULL };
			CMmsgType msgTypes [] = { CMmsgSysError, CMmsgAppError, CMmsgUsrError, CMmsgDebug, CMmsgWarning, CMmsgInfo };
			int type;
			const char *modes [] = { "file:", "on", "off", (char *) NULL };

			if ((argNum = CMargShiftLeft (argPos,argv,argNum)) < 1) {
				CMmsgPrint (CMmsgUsrError,"Missing message argument!");
				return ((CMthreadTeam_p) NULL);
			}

			if ((type = CMoptLookup (types,argv [argPos],false)) == CMfailed) {
				CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!",argv [argPos]);
			}
			else {
				switch (CMoptLookup (modes, argv [argPos] + strlen (types [type]) + 1, false)) {
					case 0: CMmsgSetStreamFile (msgTypes [type], argv [argPos] + strlen (types [type]) + 1 + strlen (modes [0]));
					case 1: CMmsgSetStatus     (msgTypes [type], true);  break;
					case 2: CMmsgSetStatus     (msgTypes [type], false); break;
					default:
						CMmsgPrint (CMmsgWarning,"Ignoring illformed message [%s]!",argv [argPos]);
						break;
				}
			}
			if ((argNum = CMargShiftLeft(argPos,argv,argNum)) <= argPos) break;
			continue;
		}
        if (CMargTest (argv[argPos], "-P", "--processor")) {
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) {
                CMmsgPrint(CMmsgUsrError, "Missing processor number!");
                return ((CMthreadTeam_p) NULL);
            }
            if (sscanf (argv[argPos],"%d", &procNum) != 1) {
                 CMmsgPrint(CMmsgUsrError, "Invalid processor number!");
                return ((CMthreadTeam_p) NULL);             
            }
            if ((argNum = CMargShiftLeft(argPos, argv, argNum)) <= argPos) break;
            continue;
        }		if (CMargTest (argv [argPos],"-h","--help")) {
			help = true;
			CMmsgPrint (CMmsgInfo,"%s [options] <domain>",CMfileName (argv [0]));
			CMmsgPrint (CMmsgInfo,"     -s,  --start      [start date in the form of \"yyyy-mm-dd\"]");
			CMmsgPrint (CMmsgInfo,"     -n,  --end        [end date in the form of \"yyyy-mm-dd\"]");
			CMmsgPrint (CMmsgInfo,"     -i,  --input      [variable=source]");
			CMmsgPrint (CMmsgInfo,"     -o,  --output     [variable=destination]");
			CMmsgPrint (CMmsgInfo,"     -t,  --state      [variable=statefile]");
			CMmsgPrint (CMmsgInfo,"     -p,  --option     [option=content]");
			CMmsgPrint (CMmsgInfo,"     -T,  --testonly");
			CMmsgPrint (CMmsgInfo,"     -m,  --message    [sys_error|app_error|usr_error|debug|warning|info]=[on|off|file=<filename>]");
			CMmsgPrint (CMmsgInfo,"     -h,  --help");
			break;
		}
		if ((argv [argPos][0] == '-') && (strlen (argv [argPos]) > 1)) {
			CMmsgPrint (CMmsgUsrError,"Unknown option [%s]!",argv [argPos]);
			_MFModelVarEntriesFree(inputVars,  inputVarNum);
			_MFModelVarEntriesFree(outputVars, outputVarNum);
			_MFModelVarEntriesFree(stateVars,  stateVarNum);
			return ((CMthreadTeam_p) NULL);
		}
		argPos++;
	}

	if (*startDate == (char *) NULL) *startDate = "XXXX-01-01";
	if (*endDate   == (char *) NULL) *endDate   = "XXXX-12-31";

    if (!MFDateSetCurrent (*startDate)) { CMmsgPrint (CMmsgUsrError,"Error: Invalid start date!"); ret = CMfailed; }
    if (!MFDateSetCurrent (*endDate))   { CMmsgPrint (CMmsgAppError,"Error: Invalid end date!");   ret = CMfailed; }
	if (ret == CMfailed) {
		_MFModelVarEntriesFree(inputVars,  inputVarNum);
		_MFModelVarEntriesFree(outputVars, outputVarNum);
		_MFModelVarEntriesFree(stateVars,  stateVarNum);
		return ((CMthreadTeam_p) NULL);
	}

	ret = mainDefFunc ();
	MFOptionPrintList ();
	if ((ret == CMfailed) || (help)) {	
		_MFModelVarEntriesFree(inputVars,  inputVarNum);
		_MFModelVarEntriesFree(outputVars, outputVarNum);
		_MFModelVarEntriesFree(stateVars,  stateVarNum);
		return ((CMthreadTeam_p) NULL);
	}

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_p) NULL;var = MFVarGetByID (++varID)) {
		if ((varEntry = _MFModelVarEntryFind (inputVars,  inputVarNum,  var->Name))  != (varEntry_p) NULL) {
            if (var->Initial) {
                var->InputPath = varEntry->Path;
                varEntry->InUse = true;
                strcpy (var->InDate, "From input");
            }
            else if (var->Set) {
                if (!testOnly) CMmsgPrint(CMmsgInfo, "Ignoring input for computed variable [%s].", var->Name);
            }
            else {
                var->InputPath = varEntry->Path;
                strcpy (var->InDate, "From input");
                var->Set = true;
                varEntry->InUse = true;
            }
        }
        else if (var->Set) strcpy (var->InDate,"Computed");
        else {
            if (!testOnly) CMmsgPrint (CMmsgInfo,"Unresolved variable: %s", var->Name);
            resolved = false;
		}
		if ((varEntry = _MFModelVarEntryFind (outputVars, outputVarNum, var->Name)) != (varEntry_p) NULL) {
			varEntry->InUse = true;
			var->OutputPath = varEntry->Path;
		}
		if ((varEntry = _MFModelVarEntryFind (stateVars,  stateVarNum,  var->Name)) != (varEntry_p) NULL) {
            if (var->Initial) {
                varEntry->InUse = true;
                var->StatePath = varEntry->Path;
                var->Set = true;
            }
            else {
                if (!testOnly) CMmsgPrint (CMmsgInfo,"Unused state file output for non initial variable %s", var->Name);
            }
		}
        else {
            if (var->Initial && !testOnly) CMmsgPrint (CMmsgInfo,"Missing output file for initial variable %s",var->Name);
        }
	}
    _MFOptionTestInUse ();
	for (i = 0; i < inputVarNum;  ++i)
		if (inputVars [i].InUse  == false) CMmsgPrint(CMmsgInfo,"Unused input variable : %s",  inputVars [i].Name);
	for (i = 0; i < outputVarNum; ++i)
		if (outputVars [i].InUse == false) CMmsgPrint(CMmsgInfo,"Unused output variable : %s", outputVars [i].Name);
	for (i = 0; i < stateVarNum;  ++i)
		if (stateVars [i].InUse  == false) CMmsgPrint(CMmsgInfo,"Unused state variable : %s",  stateVars [i].Name);
    _MFModelVarEntriesFree(inputVars,  inputVarNum);
	_MFModelVarEntriesFree(outputVars, outputVarNum);
	_MFModelVarEntriesFree(stateVars,  stateVarNum);

	if ((argNum) > 2) {
		for (i = 1; i < argNum; ++i) CMmsgPrint (CMmsgUsrError,"Argument: %s",argv[i]);
		CMmsgPrint (CMmsgUsrError,"Extra arguments!");
		return ((CMthreadTeam_p) NULL);
	}
	if (*testOnly) { _MFModelVarPrintOut ("Source"); return ((CMthreadTeam_p) NULL); }

	if ((argNum) < 2) { CMmsgPrint (CMmsgUsrError,"Missing Template Coverage!"); return ((CMthreadTeam_p) NULL); }
	*domainFile = argv [1];
	return (resolved ? CMthreadTeamCreate (procNum) : (CMthreadTeam_p) NULL);
}

static void _MFUserFunc (size_t threadId, size_t objectId, void *commonPtr) {
	int iFunc, varID, link, uLink;
	MFVariable_p var;
	float value, weight;

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_p) NULL;var = MFVarGetByID (++varID))
		if (var->Route) {
			// I -think- all WBM routed variables are considered to be extensive. Intensive variables are
			// computed within modules I THINK!. Weighing code here assumes this.
			value = 0.0;
			for (link  = 0; link < _MFDomain->Objects [objectId].ULinkNum; ++link) {
				uLink  = _MFDomain->Objects [objectId].ULinks [link];
                weight = _MFDomain->Objects [objectId].UWeights [link];
				value += MFVarGetFloat (varID,uLink,0.0) * weight;
			}
			MFVarSetFloat (varID, objectId, value);
		}
	for (iFunc = 0;iFunc < _MFFunctionNum; ++iFunc) (_MFFunctions [iFunc]) (objectId);
}

int MFModelRun (int argc, char *argv [], int argNum, int (*mainDefFunc) ()) {
	FILE *inFile;
	int i, varID, ret = CMfailed, timeStep;
    size_t * dlinks, taskId;
	char *startDate = (char *) NULL, *endDate = (char *) NULL, *domainFileName = (char *) NULL;
    const char *bifurFileName = (char *) NULL;
	char dateCur [MFDateStringLength], dateNext [MFDateStringLength], *climatologyStr;
	bool testOnly;
    void *buffer, *status;
	MFVariable_p var;
	time_t sec;
	CMthreadTeam_p team = (CMthreadTeam_p) NULL;
 	CMthreadJob_p  job  = (CMthreadJob_p)  NULL;
    pthread_attr_t thread_attr;

	team = _MFModelParse (argc,argv,argNum, mainDefFunc, &domainFileName, &startDate, &endDate, &testOnly);
 	if (testOnly) return (CMsucceeded);
    if (team == (CMthreadTeam_p) NULL) return (CMfailed);

    switch (strlen (startDate)) {
        case  4: timeStep = MFTimeStepYear;  climatologyStr = MFDateClimatologyYearStr;  break;
        case  7: timeStep = MFTimeStepMonth; climatologyStr = MFDateClimatologyMonthStr; break;
        case 10: timeStep = MFTimeStepDay;   climatologyStr = MFDateClimatologyDayStr;   break;
        case 13: timeStep = MFTimeStepHour;  climatologyStr = MFDateClimatologyHourStr;  break;
        default: CMmsgPrint (CMmsgUsrError,"Invalid date in model run %s\n", startDate); goto Stop;
    }
    if (!MFDateSetCurrent (startDate)) { CMmsgPrint (CMmsgUsrError,"Error: Invalid start date!"); ret = CMfailed; }
    if ((inFile = strcmp (domainFileName,"-") != 0 ? fopen (argv [1],"r") : stdin) == (FILE *) NULL) {
		CMmsgPrint (CMmsgAppError,"%s: Template Coverage [%s] Opening error!",CMfileName (argv [0]),argv [1]);
		goto Stop;
	}
	if ((_MFDomain = MFDomainRead (inFile)) == (MFDomain_p) NULL) { ret = CMfailed; goto Stop; }
   if ((bifurFileName = MFOptionGet(MFBifurcationOpt)) != (char *) NULL) {
        if (MFDomainSetBifurcations(_MFDomain, bifurFileName) == CMfailed) { ret = CMfailed; goto Stop; }
    }

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_p) NULL;var = MFVarGetByID (++varID)) {
		var->ItemNum = _MFDomain->ObjNum;
        if (var->InputPath != (char *) NULL) {
            if ((var->InStream = MFDataStreamOpen(var->InputPath, "r")) == (MFDataStream_p) NULL) goto Stop;
            if (var->Initial) {
                strcpy (var->InDate, climatologyStr);
                var->Read = true;
                if (MFdsRecordRead(var)              == CMfailed) goto Stop;
                if (MFDataStreamClose(var->InStream) == CMfailed) goto Stop;
                var->InStream   = (MFDataStream_p) NULL;
            }
            else {
                strcpy (var->InDate, startDate);
                var->Read = true;
                if (MFdsRecordRead(var) == CMfailed) goto Stop;
            }
        }
		else {
            var->TStep = (short) timeStep;
            switch (var->Type) {
                case MFInput:
                    CMmsgPrint (CMmsgAppError, "Error: Unresolved variable (%s [%s] %s)!",var->Name,var->Unit, MFVarTypeString (var->Type));
                    ret = CMfailed;
                    break;
                case MFRoute:
                case MFOutput:
                    var->Type = MFFloat;
                    var->Missing.Float = MFDefaultMissingFloat;
                default: strcpy (var->InDate,"computed"); break;
            }
            if ((var->Buffer = (void *) calloc(var->ItemNum, MFVarItemSize(var->Type))) == (void *) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: %s:%d", __FILE__, __LINE__);
                goto Stop;
            }
            for (i = 0; i < var->ItemNum; ++i) MFVarSetFloat(var->ID,i,0.0);
        }
        if (var->Flux) sprintf (var->Unit + strlen(var->Unit), "/%s", MFDateTimeStepUnit(var->TStep));
        if (var->OutputPath != (char *) NULL) {
            if ((var->OutStream = MFDataStreamOpen(var->OutputPath,"w")) == (MFDataStream_p) NULL) { ret = CMfailed; goto Stop; }
        }
	}
    _MFModelVarPrintOut ("Start date");
	if (ret == CMfailed) goto Stop;

    if ((job = CMthreadJobCreate(_MFDomain->ObjNum, _MFUserFunc, (void *) NULL)) == (CMthreadJob_p) NULL) {
        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d", __FILE__, __LINE__);
        CMthreadTeamDelete(team);
        goto Stop;
    }
    else {
        for (taskId = 0; taskId < _MFDomain->ObjNum; ++taskId) {
            dlinks  = _MFDomain->Objects[taskId].DLinkNum > 0 ? _MFDomain->Objects[taskId].DLinks : (size_t *) &taskId;
            CMthreadJobTaskDependent(job, taskId, dlinks, _MFDomain->Objects[taskId].DLinkNum);
        }
    }
    time(&sec);
    strcpy (dateCur,  MFDateGetCurrent ());
    strcpy (dateNext, MFDateGetNext ());
    CMmsgPrint (CMmsgInfo, "Model run started at... %s  started at %.24s", dateCur, ctime(&sec));

    do {
        CMmsgPrint(CMmsgDebug, "Computing: %s", dateCur);

        CMthreadJobExecute (team, job);
        for (var = MFVarGetByID(varID = 1); var != (MFVariable_p) NULL; var = MFVarGetByID(++varID)) {
            strcpy (var->OutDate, dateCur);
            if (var->OutStream != (MFDataStream_p) NULL) {
                if ((ret = MFdsRecordWrite(var)) == CMfailed) {
                    CMmsgPrint(CMmsgAppError, "Variable (%s) writing error!", var->Name);
                    goto Stop;
                }
            }
            if (var->InStream != (MFDataStream_p) NULL) {
                if ((MFDateCompare(startDate, dateNext) < 0) && (MFDateCompare(dateNext,endDate) <= 0)) {
                    strcpy (var->InDate, dateNext);
                    if ((ret = MFdsRecordRead(var)) == CMfailed) {
                        CMmsgPrint(CMmsgAppError, "Variable (%s) Reading error!", var->Name);
                        goto Stop;
                    }
                }
            }
            else strcpy (var->CurDate,dateCur);
        }
        strcpy (dateCur,  dateNext);
        MFDateSetCurrent(dateCur);
        strcpy (dateNext, MFDateGetNext ());
    } while ((MFDateCompare(startDate, dateCur) < 0) && (MFDateCompare (dateCur,endDate) <= 0));

	for (var = MFVarGetByID (varID = 1);var != (MFVariable_p) NULL;var = MFVarGetByID (++varID)) {
		if (var->InStream  != (MFDataStream_p) NULL) MFDataStreamClose (var->InStream);
		if (var->OutStream != (MFDataStream_p) NULL) MFDataStreamClose (var->OutStream);
        if (var->StatePath != (char *) NULL) {
            if ((var->OutStream = MFDataStreamOpen (var->StatePath,"w")) == (MFDataStream_p) NULL) ret = CMfailed;
            strcpy (var->OutDate,dateCur);
			if (MFdsRecordWrite(var) == CMfailed) {
                CMmsgPrint (CMmsgAppError,"Variable (%s) writing error!",var->Name);
                goto Stop;
            }
			MFDataStreamClose (var->OutStream);
		}
		free (var->Buffer);
	}
	ret = CMsucceeded;
Stop:
    if (job  != (CMthreadJob_p)  NULL) CMthreadJobDestroy (job);
	if (team != (CMthreadTeam_p) NULL) {
	    CMthreadTeamPrintReport (CMmsgInfo, team);
    	CMthreadTeamDelete (team);
	}
	return (ret);
}
