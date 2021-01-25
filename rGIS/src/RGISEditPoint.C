/******************************************************************************

GHAAS RiverGIS V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

RGISEditPoint.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <Xm/Label.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/Frame.h>
#include <Xm/Scale.h>
#include <rgis.H>

#define RGISEditPointXCoord "XCoord"
#define RGISEditPointYCoord "YCoord"

void RGISEditPointAddXYCBK (Widget widget, RGISWorkspace *workspace,XmAnyCallbackStruct *callData)

	{
	DBDataset *dataset = UIDataset ();
	DBObjData *dbData =dataset->Data ();
	DBVPointIF *pntIF = new DBVPointIF (dbData);
	DBObjTable *pointTable = dbData->Table (DBrNItems);
	DBObjTableField *xCoordFLD	= pointTable->Field (RGISEditPointXCoord);
	DBObjTableField *yCoordFLD = pointTable->Field (RGISEditPointYCoord);
	DBCoordinate coord;
	DBObjRecord *pointRec;
	UITable *tableCLS = (UITable *) dbData->Display (UITableName (dbData,pointTable));

	UIPauseDialogOpen ((char *) "Adding XY Coordinates");
	if (xCoordFLD == NULL)
		{
		xCoordFLD = new DBObjTableField (RGISEditPointXCoord,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (xCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (xCoordFLD);
		UIPause (40);
		}
	if (yCoordFLD == NULL)
		{
		yCoordFLD = new DBObjTableField (RGISEditPointYCoord,DBTableFieldFloat,"%10.3f",sizeof (DBFloat4));
		pointTable->AddField (yCoordFLD);
		if (tableCLS != (UITable *) NULL) tableCLS->AddField (yCoordFLD);
		UIPause (80);
		}
	for (pointRec = pntIF->FirstItem (); pointRec != (DBObjRecord *) NULL; pointRec = pntIF->NextItem ())
		{
		if (UIPause (80 + pointRec->RowID () * 20 / pntIF->ItemNum ())) goto Stop;
		coord = pntIF->Coordinate  (pointRec);
		xCoordFLD->Float (pointRec,coord.X);
		yCoordFLD->Float (pointRec,coord.Y);
		}
Stop:
	UIPauseDialogClose ();
	if (tableCLS != (UITable *) NULL) tableCLS->Draw ();
	}

static void _RGISEditPointSTNCoordsSelectCBK (Widget widget,Widget text,XmAnyCallbackStruct *callData)

	{
	static Widget select = NULL;
	char *field;
	DBObjectLIST<DBObjTableField> *fields;

	if (select == NULL) select = UISelectionCreate ((char *) "Field Selection");
	XtVaGetValues (text,XmNuserData, &fields, NULL);
	if ((field = UISelectObject (select,(DBObjectLIST<DBObject> *) fields,DBTableFieldIsFloat)) != NULL)
			XmTextFieldSetString (text,field);
	else	XmTextFieldSetString (text,(char *) "");
	}

static void _RGISEditPointSTNCoordsScaleCBK (Widget widget, void *data, XmScaleCallbackStruct *callData) {
    char numberString[13];
	DBInt scaleMultiplier = *((DBInt *) callData);
    Widget label;

    XtVaGetValues(widget, XmNuserData, &label, NULL);
    sprintf(numberString,"%d", callData->value * scaleMultiplier);
    UIAuxSetLabelString(label, numberString);
}


void RGISEditPointSTNCoordsCBK (Widget widget,void *data,XmAnyCallbackStruct *callData)

	{
	char *text, numberString[13];
	static DBInt cont, maxRadius = 1, tolerance = 10, toleranceMultiplier = 1, radiusMultiplier = 10;
	DBDataset *dataset;
	DBObjData *dbData, *netData;
	DBObjTable *sTable, *cTable;
	DBObjTableField *field = (DBObjTableField *) NULL;
	static Widget dShell = NULL, mainForm, button, textF, frame, toleranceLabel, toleranceScale, pRadiusLabel, pRadiusScale;

	if (dShell == (Widget) NULL)
		{
		XmString string;

		dShell = UIDialogForm ((char *) "Area Field",false);
		mainForm = UIDialogFormGetMainForm (dShell);

		string = XmStringCreate ((char *) "Select",UICharSetBold);
		button = XtVaCreateManagedWidget ("RGISEditSTNCoordButton",xmPushButtonWidgetClass,mainForm,
										XmNtopAttachment,    XmATTACH_FORM,
										XmNtopOffset,        10,
										XmNrightAttachment,  XmATTACH_FORM,
										XmNrightOffset,      10,
										XmNmarginHeight,     5,
										XmNtraversalOn,      False,
										XmNlabelString,      string,
										XmNuserData,         DBTableFieldIsCategory,
										NULL);
		XmStringFree (string);
		textF = XtVaCreateManagedWidget ("RGISEditSTNCoordTextF",xmTextFieldWidgetClass,mainForm,
										XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
										XmNtopWidget,        button,
										XmNrightAttachment,  XmATTACH_WIDGET,
										XmNrightWidget,      button,
										XmNrightOffset,      10,
										XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
										XmNbottomWidget,     button,
										XmNmaxLength,        DBStringLength,
										XmNcolumns,          DBStringLength / 2,
										NULL);
		XtAddCallback (button,XmNactivateCallback,(XtCallbackProc) _RGISEditPointSTNCoordsSelectCBK,textF);
		string = XmStringCreate ((char *) "Area Field:",UICharSetBold);
		XtVaCreateManagedWidget ("RGISEditSTNCoordAreaFieldNameLabel",xmLabelWidgetClass,mainForm,
										XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
										XmNtopWidget,        button,
										XmNleftAttachment,   XmATTACH_FORM,
										XmNleftOffset,       10,
										XmNrightAttachment,  XmATTACH_WIDGET,
										XmNrightWidget,      textF,
										XmNrightOffset,      10,
										XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
										XmNbottomWidget,     button,
										XmNlabelString,      string,
										NULL);
		XmStringFree (string);
        frame = XtVaCreateManagedWidget("RGISEditSTNCoordToleranceDisplayScale", xmFrameWidgetClass, mainForm,
                                        XmNtopAttachment,   XmATTACH_WIDGET,
                                        XmNtopWidget,       button,
                                        XmNtopOffset,       5,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNrightOffset,     5,
                                        NULL);
		string = XmStringCreate((char *) "NotSet", UICharSetNormal);
        toleranceLabel = XtVaCreateManagedWidget("RGISEditSTNCoordToleranceDisplayLabel", xmLabelWidgetClass, frame,
                                        XmNmarginWidth,     5,
                                        XmNalignment,       XmALIGNMENT_END,
                                        XmNlabelString,     string,
                                        XmNrecomputeSize,   false,
                                        NULL);
        XmStringFree(string);
		toleranceScale = XtVaCreateManagedWidget("RGISEditSTNCoordPixelRadiusNameScale", xmScaleWidgetClass, mainForm,
                                        XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                        XmNtopWidget,        frame,
                                        XmNtopOffset,        2,
                                        XmNrightAttachment,  XmATTACH_WIDGET,
                                        XmNrightWidget,      frame,
                                        XmNrightOffset,      10,
                                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                                        XmNbottomWidget,     frame,
                                        XmNbottomOffset,     2,
                                        XmNorientation,      XmHORIZONTAL,
                                        XmNminimum,          1,
                                        XmNmaximum,          50,
                                        XmNvalue,            tolerance,
                                        XmNscaleWidth,       110,
                                        XmNtraversalOn,      false,
                                        XmNuserData,         toleranceLabel,
                                    	NULL);
		toleranceMultiplier = 1;
        XtAddCallback(toleranceScale, XmNdragCallback,         (XtCallbackProc) _RGISEditPointSTNCoordsScaleCBK, (void *) &toleranceMultiplier);
        XtAddCallback(toleranceScale, XmNvalueChangedCallback, (XtCallbackProc) _RGISEditPointSTNCoordsScaleCBK, (void *) &toleranceMultiplier);

        string = XmStringCreate((char *) "Tolerance [%]:", UICharSetBold);
        XtVaCreateManagedWidget("RGISEditSTNCoordTolleranceNameLabel", xmLabelWidgetClass, mainForm,
                                        XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET,
                                        XmNtopWidget,       frame,
                                        XmNtopOffset,       5,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNlabelString,     string,
                                        NULL);
        XmStringFree(string);
        frame = XtVaCreateManagedWidget("RGISEditSTNCoordPixelRadiusDisplayScale", xmFrameWidgetClass, mainForm,
                                        XmNtopAttachment,   XmATTACH_WIDGET,
                                        XmNtopWidget,       frame,
                                        XmNtopOffset,       5,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNrightOffset,     5,
                                        NULL);
		string = XmStringCreate((char *) "NotSet", UICharSetNormal);
        pRadiusLabel = XtVaCreateManagedWidget("RGISEditSTNCoordPixelRadiusDisplayLabel", xmLabelWidgetClass, frame,
                                        XmNmarginWidth,     5,
                                        XmNalignment,       XmALIGNMENT_END,
                                        XmNlabelString,     string,
                                        XmNrecomputeSize,   false,
                                        NULL);
        XmStringFree(string);
		pRadiusScale = XtVaCreateManagedWidget("RGISEditSTNCoordPixelRadiusNameScale", xmScaleWidgetClass, mainForm,
                                        XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                        XmNtopWidget,        frame,
                                        XmNtopOffset,        2,
                                        XmNrightAttachment,  XmATTACH_WIDGET,
                                        XmNrightWidget,      frame,
                                        XmNrightOffset,      10,
                                        XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                                        XmNbottomWidget,     frame,
                                        XmNbottomOffset,     2,
                                        XmNorientation,      XmHORIZONTAL,
                                        XmNminimum,          1,
                                        XmNmaximum,          50,
                                        XmNvalue,            maxRadius,
										XmNscaleMultiple,    5,
                                        XmNscaleWidth,       110,
                                        XmNtraversalOn,      false,
                                        XmNuserData,         pRadiusLabel,
                                    	NULL);
		radiusMultiplier = 10;
        XtAddCallback(pRadiusScale, XmNdragCallback,         (XtCallbackProc) _RGISEditPointSTNCoordsScaleCBK, (void *) &radiusMultiplier);
        XtAddCallback(pRadiusScale, XmNvalueChangedCallback, (XtCallbackProc) _RGISEditPointSTNCoordsScaleCBK, (void *) &radiusMultiplier);

        string = XmStringCreate((char *) "Max. Radius [km]:", UICharSetBold);
        XtVaCreateManagedWidget("RGISEditSTNCoordPixelRadiusNameLabel", xmLabelWidgetClass, mainForm,
                                        XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET,
                                        XmNtopWidget,       frame,
                                        XmNtopOffset,       5,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNlabelString,     string,
                                        NULL);
        XmStringFree(string);

		XtAddCallback (UIDialogFormGetOkButton (dShell),XmNactivateCallback,(XtCallbackProc) UIAuxSetBooleanTrueCBK,&cont);
		XtSetSensitive (UIDialogFormGetOkButton (dShell),true);
		}

	dataset = UIDataset ();
	dbData  = dataset->Data ();
	netData = dbData->LinkedData();
	sTable  = dbData->Table (DBrNItems);
	cTable  = netData->Table (DBrNCells);
	XtVaSetValues (textF,XmNuserData,sTable->Fields (),NULL);
	sprintf(numberString,"%d", tolerance * toleranceMultiplier);
    UIAuxSetLabelString(toleranceLabel, numberString);
	XmScaleSetValue(toleranceScale, tolerance * toleranceMultiplier);
	sprintf(numberString,"%d", maxRadius * radiusMultiplier);
    UIAuxSetLabelString(pRadiusLabel, numberString);
	XmScaleSetValue(pRadiusScale, maxRadius * radiusMultiplier);

	UIDialogFormPopup (dShell);
	cont = false;
	while (UILoop ())
		{
		if (strlen (text = XmTextFieldGetString (textF)) > 0)
		field = sTable->Field (text);
		XmScaleGetValue(toleranceScale, &tolerance);
		XmScaleGetValue(pRadiusScale,   &maxRadius);
		XtFree (text);
		}

	UIDialogFormPopdown (dShell);
	if (cont)
		{
		UIPauseDialogOpen ((char *) "Moving Points");
		RGlibPointSTNCoordinates (dbData,field,cTable->Field(DBrNSubbasinArea),(DBFloat) tolerance / 100.0, maxRadius * radiusMultiplier, true);
		UIPauseDialogClose ();
		UI2DViewRedrawAll ();
		}
	}
