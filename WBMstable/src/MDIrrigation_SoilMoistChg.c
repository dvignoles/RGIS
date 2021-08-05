/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDIrrSoilMoisture.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrSoilMoistChgID = MFUnset;

int MDIrrigation_SoilMoistChgDef() {
	int optID = MFnone, ret;
	const char *optStr;

	if (_MDOutIrrSoilMoistChgID != MFUnset) return (_MDOutIrrSoilMoistChgID);
	if ((optStr = MFOptionGet (MDOptConfig_Irrigation)) != (char *) NULL) optID = CMoptLookup (MFcalcOptions, optStr, true);
		
	switch (optID) {
		default:      return CMfailed;
		case MFhelp:
		case MFnone: 
		case MFinput: break;
		case MFcalculate: 
			if ((ret = MDIrrigation_GrossDemandDef ()) == CMfailed) return CMfailed;
			_MDOutIrrSoilMoistChgID = ret != MFUnset ? MFVarGetID (MDVarIrrigation_SoilMoistChange, "mm", MFInput, MFFlux, MFBoundary) : MFUnset;
			break;
	}
    return (_MDOutIrrSoilMoistChgID);
}
