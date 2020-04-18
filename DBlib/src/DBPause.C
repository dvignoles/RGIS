/******************************************************************************

GHAAS Database library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

DBPause.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <DB.H>

static int (*_DBPauseFunction)(int) = NULL;

void DBPauseSetFunction(int (*pauseFunc)(int)) { _DBPauseFunction = pauseFunc; }

int DBPause(int perCent) {
    if (_DBPauseFunction == NULL) return (false);
    else return ((*_DBPauseFunction)(perCent));
}
