/******************************************************************************

GHAAS RiverGIS V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

RGISEditPoly.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <rgis.hpp>

void RGISEditPolyFourColorCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBVPolyIF *polyIF = new DBVPolyIF (dataset->Data ());

	UIPauseDialogOpen ((char *) "Four Coloring Polygons");
	polyIF->FourColoring ();
	UIPauseDialogClose ();
	}
