/******************************************************************************

GHAAS RiverGIS Libarary V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2022, UNH - ASRC/CUNY

RGlibDataStream.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include<cm.h>
#include<DB.hpp>
#include<DBif.hpp>
#include<MF.h>

static void _RGlibRGIS2DataStreamPointFunc   (size_t threadId, size_t objectId, void *commonPtr);
static void _RGlibRGIS2DataStreamGridFunc    (size_t threadId, size_t objectId, void *commonPtr);
static void _RGlibRGIS2DataStreamNetworkFunc (size_t threadId, size_t objectId, void *commonPtr);

class RGlibRGIS2DataStreamThreadData {
    private:
        DBInt ItemSize;
        void *Data;
        MFdsHeader_t DSHeader;
        DBGridIF *GridIF;
        DBGridSampler *Sampler;
        DBObjRecord *LayerRec;
        bool TeamInitialized;
        CMthreadTeam_t Team;
 	    CMthreadJob_p  Job;
        union {
                void        *Any;
                DBVPointIF  *PointIF;
                DBGridIF    *GridIF;
                DBNetworkIF *NetIF;
        } Interface;
    public:
        DBInt Init (DBObjData *tmplData, DBObjData *grdData) {
            Data = (void *) NULL;
            Interface.Any = (void *) NULL;
            Sampler = (DBGridSampler *) NULL;
            DSHeader.Swap = 1;
            GridIF = new DBGridIF(grdData);
            ItemSize = GridIF->ValueSize();
            DBObjRecord *LayerRec = (DBObjRecord *) NULL;
            TeamInitialized = false;
            Job = (CMthreadJob_p) NULL;
            switch (GridIF->ValueType()) {
                case DBVariableInt:
                    switch (ItemSize) {
                        case 1: DSHeader.Type = MFByte;  break;
                        case 2: DSHeader.Type = MFShort; break;
                        case 4: DSHeader.Type = MFInt;   break;
                        default: CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__); return (DBFault);
                        }
                    DSHeader.Missing.Int = (int) GridIF->MissingValue();
                    break;
                case DBVariableFloat:
                    switch (ItemSize) {
                        case 4: DSHeader.Type = MFFloat;  break;
                        case 8: DSHeader.Type = MFDouble; break;
                        default: CMmsgPrint(CMmsgAppError, "Error: Invalid field size in: %s %d", __FILE__, __LINE__); return (DBFault);
                        }
                    DSHeader.Missing.Float = GridIF->MissingValue();
                    break;
                default: CMmsgPrint(CMmsgAppError, "Error: Invalid field type in: %s %d", __FILE__, __LINE__); return (DBFault);
            }
            TeamInitialized = true;
            switch (tmplData != (DBObjData *) NULL ? tmplData->Type () : grdData->Type ()) {
                case DBTypeVectorPoint:
                    Interface.PointIF = new DBVPointIF(tmplData);
                    DSHeader.ItemNum = Interface.PointIF->ItemNum();
                    if ((DSHeader.Type == MFFloat) || (DSHeader.Type == MFDouble)) { // Using sampler
                        DBInt itemID;
                        DBObjRecord *pntRec;
                        if ((Sampler = (DBGridSampler *) calloc(sizeof(DBGridSampler), (size_t) DSHeader.ItemNum)) == (DBGridSampler *) NULL) {
                            CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                            }
                        for (itemID = 0; itemID < DSHeader.ItemNum; ++itemID) {
                            pntRec = Interface.PointIF->Item(itemID);
                            GridIF->Coord2Sampler(Interface.PointIF->Coordinate(pntRec), Sampler[itemID]);
                        }
                    } else Sampler = (DBGridSampler *) NULL; // Not using sampler
                    if ((Job = CMthreadJobCreate(DSHeader.ItemNum, _RGlibRGIS2DataStreamPointFunc, (void *) this)) == (CMthreadJob_p) NULL) {
                        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    break;
                case DBTypeGridContinuous: 
                case DBTypeGridDiscrete:
                    Interface.GridIF = tmplData != (DBObjData *) NULL ? new DBGridIF(tmplData) : GridIF;
                    DSHeader.ItemNum = Interface.GridIF->RowNum() * Interface.GridIF->ColNum();
                    if ((Interface.GridIF != GridIF) && ((DSHeader.Type == MFFloat) || (DSHeader.Type == MFDouble))) { // Using sampler
                        DBInt itemID;
                        DBPosition pos;
                        DBCoordinate coord;
                        if ((Sampler = (DBGridSampler *) calloc(sizeof(DBGridSampler), (size_t) DSHeader.ItemNum)) == (DBGridSampler *) NULL) {
                            CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                        }
                        for (pos.Row = 0; pos.Row < Interface.GridIF->RowNum(); ++pos.Row) {
                            for (pos.Col = 0; pos.Col < Interface.GridIF->ColNum(); ++pos.Col) {
                                itemID = pos.Row * Interface.GridIF->ColNum() + pos.Col;
                                Interface.GridIF->Pos2Coord(pos, coord);
                                GridIF->Coord2Sampler(coord, Sampler[itemID]);
                            }
                        }
                    } else Sampler = (DBGridSampler *) NULL; // Not using sampler
                    if ((Job = CMthreadJobCreate(DSHeader.ItemNum, _RGlibRGIS2DataStreamGridFunc, (void *) this)) == (CMthreadJob_p) NULL) {
                        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    break;
                case DBTypeNetwork:
                    Interface.NetIF = new DBNetworkIF(tmplData);
                    DSHeader.ItemNum = Interface.NetIF->CellNum();
                    if ((DSHeader.Type == MFFloat) || (DSHeader.Type == MFDouble)) { // Using sampler
                        DBInt itemID;
                        DBObjRecord *cellRec;
                        if ((Sampler = (DBGridSampler *) calloc (sizeof(DBGridSampler), (size_t) DSHeader.ItemNum)) == (DBGridSampler *) NULL) {
                            CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                        }
                        for (itemID = 0; itemID < Interface.NetIF->CellNum(); ++itemID) {
                            cellRec = Interface.NetIF->Cell(itemID);
                            GridIF->Coord2Sampler(Interface.NetIF->Center(cellRec), Sampler[itemID]);
                        }
                    } else Sampler = (DBGridSampler *) NULL; // Not using sampler
                    if ((Job = CMthreadJobCreate(DSHeader.ItemNum, _RGlibRGIS2DataStreamNetworkFunc, (void *) this)) == (CMthreadJob_p) NULL) {
                        CMmsgPrint(CMmsgAppError, "Job creation error in %s:%d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    break;
            }
            if ((Data = calloc((size_t) (DSHeader.ItemNum), (size_t) ItemSize)) == (void *) NULL) {
                CMmsgPrint(CMmsgSysError, "Error! Allocating %d items of %d size in: %s %d", DSHeader.ItemNum, ItemSize, __FILE__, __LINE__);
                return (DBFault);
            }
            if (CMthreadTeamInitialize (&Team,CMthreadProcessorNum (),DSHeader.ItemNum) == (CMthreadTeam_p) NULL) {
                CMmsgPrint (CMmsgUsrError,"Team initialization error %s, %d",__FILE__,__LINE__);
                return (DBFault);
            }
            return (DBSuccess);
        }

        void ComputePoint (DBInt itemID) {
            DBInt intValue;
            DBFloat floatValue;
            if (Sampler == (DBGridSampler *) NULL) {
                DBObjRecord *pntRec;

                pntRec = Interface.PointIF->Item(itemID);
                if (DSHeader.Type == MFFloat || DSHeader.Type == MFDouble) {
                    if (GridIF->Value(LayerRec, Interface.PointIF->Coordinate(pntRec), &floatValue) == false) floatValue = DSHeader.Missing.Float;
                } else {
                    if (GridIF->Value(LayerRec, Interface.PointIF->Coordinate(pntRec), &intValue)   == false) intValue   = DSHeader.Missing.Int;
                }
            }
            else {
                if (GridIF->Value(LayerRec, Sampler [itemID], &floatValue) == false) floatValue = DSHeader.Missing.Float;
            }
            switch (DSHeader.Type) {
                case MFByte:   ((char *)   Data) [itemID] = (char)   intValue;   break;
                case MFShort:  ((short *)  Data) [itemID] = (short)  intValue;   break;
                case MFInt:    ((int *)    Data) [itemID] = (short)  intValue;   break;
                case MFFloat:  ((float *)  Data) [itemID] = (float)  floatValue; break;
                case MFDouble: ((double *) Data) [itemID] = (double) floatValue; break;
            }
        }
        void ComputeGrid (DBInt itemID) {
            DBInt intValue;
            DBFloat floatValue;

            if (Sampler == (DBGridSampler *) NULL) {
                DBPosition pos;
        
                pos.Row = itemID / Interface.GridIF->ColNum ();
                pos.Col = itemID % Interface.GridIF->ColNum ();
                if (Interface.GridIF != GridIF) {
                    DBCoordinate coord;
                    Interface.GridIF->Pos2Coord(pos, coord);
                    if (DSHeader.Type == MFFloat || DSHeader.Type == MFDouble) {
                        if (GridIF->Value(LayerRec, coord, &floatValue) == false) floatValue = DSHeader.Missing.Float;
                    } else {
                        if (GridIF->Value(LayerRec, coord, &intValue)   == false) intValue   = DSHeader.Missing.Int;
                    }
                } else {
                    if (DSHeader.Type == MFFloat || DSHeader.Type == MFDouble) {
                        if (GridIF->Value(LayerRec, pos,   &floatValue) == false) floatValue = DSHeader.Missing.Float;
                    }
                    else {
                        if (GridIF->Value(LayerRec, pos,   &intValue)   == false) intValue   = DSHeader.Missing.Int;
                    }
                }
            } else {
                if (GridIF->Value(LayerRec, Sampler [itemID], &floatValue) == false) floatValue = DSHeader.Missing.Float;
            }
            switch (DSHeader.Type) {
                case MFByte:   ((char *)   Data)[itemID] = (char)   intValue;   break;
                case MFShort:  ((short *)  Data)[itemID] = (short)  intValue;   break;
                case MFInt:    ((int *)    Data)[itemID] = (int)    intValue;   break;
                case MFFloat:  ((float *)  Data)[itemID] = (float)  floatValue; break;
                case MFDouble: ((double *) Data)[itemID] = (double) floatValue; break;
            }
        }
        void ComputeNetwork (DBInt itemID) {
            DBInt   intValue;
            DBFloat floatValue;

            if (Sampler == (DBGridSampler *) NULL) {
                DBObjRecord *cellRec;
                cellRec = Interface.NetIF->Cell(itemID);
                if (DSHeader.Type == MFFloat || DSHeader.Type == MFDouble) {
                    if (GridIF->Value(LayerRec, Interface.NetIF->Center(cellRec), &floatValue) == false) floatValue = DSHeader.Missing.Float;    
                }
                else {
                    if (GridIF->Value(LayerRec, Interface.NetIF->Center(cellRec), &intValue)   == false) intValue   = DSHeader.Missing.Int;
                }
            }
            else {
                if (GridIF->Value(LayerRec, Sampler[itemID], &floatValue) == false) floatValue = DSHeader.Missing.Float;
            }
            switch (DSHeader.Type) {
                case MFByte:   ((char *)   Data) [itemID] = (char)   intValue;   break;
                case MFShort:  ((short *)  Data) [itemID] = (short)  intValue;   break;
                case MFInt:    ((int *)    Data) [itemID] = (int)    intValue;   break;
                case MFFloat:  ((float *)  Data) [itemID] = (float)  floatValue; break;
                case MFDouble: ((double *) Data) [itemID] = (double) floatValue; break;
            }
        }
        DBInt Run (FILE *outFile) {
            DBInt layerID;

            for (layerID = 0; layerID < GridIF->LayerNum(); ++layerID) {
                LayerRec = GridIF->Layer(layerID);
                CMthreadJobExecute(&Team, Job);
                strncpy(DSHeader.Date, LayerRec->Name(), MFDateStringLength - 1);
                if ((DBInt) fwrite(&DSHeader, sizeof(MFdsHeader_t), 1, outFile) != 1) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing record header in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }
                if ((DBInt) fwrite(Data, (size_t) ItemSize, (size_t) (DSHeader.ItemNum), outFile) != DSHeader.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Writing data in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }
            }
            return (DBSuccess);
        }
        void  Finalize (DBObjData *tmplData) {
            if (TeamInitialized) CMthreadTeamDestroy(&Team);
            if (Job != (CMthreadJob_p) NULL) CMthreadJobDestroy  (Job);
            if ((tmplData != (DBObjData *) NULL) && (Interface.Any != (void *) NULL)) {
                switch (tmplData->Type ()) {
                    case DBTypeVectorPoint:    delete Interface.PointIF; break;
                    case DBTypeGridContinuous: 
                    case DBTypeGridDiscrete:   delete Interface.GridIF;  break;
                    case DBTypeNetwork:        delete Interface.NetIF;   break;
                }
            }
            if (Sampler != (DBGridSampler *) NULL) free (Sampler);
            if (Data    != (void *) NULL)          free (Data);
            delete GridIF;
        }
};

static void _RGlibRGIS2DataStreamPointFunc (size_t threadId, size_t objectId, void *commonPtr) {
	RGlibRGIS2DataStreamThreadData *threadData = (RGlibRGIS2DataStreamThreadData *) commonPtr;
	threadData->ComputePoint (objectId); 
}

static void _RGlibRGIS2DataStreamGridFunc (size_t threadId, size_t objectId, void *commonPtr) {
	RGlibRGIS2DataStreamThreadData *threadData = (RGlibRGIS2DataStreamThreadData *) commonPtr;
	threadData->ComputeGrid (objectId); 
}

static void _RGlibRGIS2DataStreamNetworkFunc (size_t threadId, size_t objectId, void *commonPtr) {
	RGlibRGIS2DataStreamThreadData *threadData = (RGlibRGIS2DataStreamThreadData *) commonPtr;
	threadData->ComputeNetwork (objectId); 
}

DBInt RGlibRGIS2DataStream(DBObjData *grdData, DBObjData *tmplData, FILE *outFile) {
    RGlibRGIS2DataStreamThreadData dataStreamData;

    if (dataStreamData.Init (tmplData, grdData) != DBSuccess) { dataStreamData.Finalize (tmplData); return (DBFault); }
    if (dataStreamData.Run  (outFile) != DBSuccess)           { dataStreamData.Finalize (tmplData); return (DBFault); }
    dataStreamData.Finalize (tmplData);
    return (DBSuccess);
}

DBInt RGlibDataStream2RGIS(DBObjData *outData, DBObjData *tmplData, FILE *inFile) {
    DBInt layerID = 0, itemSize;
    DBPosition pos;
    DBFloat val;
    void *data = (void *) NULL;
    MFdsHeader_t header;
    DBObjRecord *record;

    switch (tmplData->Type()) {
        case DBTypeVectorPoint: {
            DBInt itemID;
            DBDate date;
            DBObjTable *itemTable = outData->Table(DBrNItems);
            DBObjTableField *idField = new DBObjTableField("ItemID", DBTableFieldInt, "%6d", sizeof(DBInt), false);
            DBObjTableField *dateField = new DBObjTableField("Date", DBTableFieldDate, "%s", sizeof(DBDate), false);
            DBObjTableField *valField;
            DBVPointIF *pntIF = new DBVPointIF(tmplData);

            itemTable->AddField(idField);
            itemTable->AddField(dateField);

            while (MFdsHeaderRead (&header, inFile) == CMsucceeded) {
                if (header.ItemNum != pntIF->ItemNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency %d %d!", header.ItemNum,
                               pntIF->ItemNum());
                    return (DBFault);
                }
                if (data == (void *) NULL) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = (void *) realloc(data, (size_t) header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    switch (header.Type) {
                        case MFByte:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%2d", sizeof(char), false); break;
                        case MFShort:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%4d", sizeof(DBShort), false); break;
                        case MFInt:
                            valField = new DBObjTableField("Value", DBTableFieldInt, "%8d", sizeof(DBInt), false); break;
                        case MFFloat:
                            valField = new DBObjTableField("Value", DBTableFieldFloat, "%8.2f", sizeof(DBFloat4), false); break;
                        case MFDouble:
                            valField = new DBObjTableField("Value", DBTableFieldFloat, "%8.2f", sizeof(DBFloat), false); break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                    itemTable->AddField(valField);
                }
                if ((int) fread(data, (size_t) itemSize, (size_t) (header.ItemNum), inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }
                for (itemID = 0; itemID < header.ItemNum; ++itemID) {
                    record = itemTable->Add(header.Date);
                    date.Set(header.Date);
                    idField->Int(record, itemID);
                    dateField->Date(record, date);
/*					decDateField->Float (record,date);
*/                    switch (header.Type) {
                        case MFByte:
                            valField->Int(record, ((char *) data)[itemID]);
                            break;
                        case MFShort:
                            valField->Int(record, ((short *) data)[itemID]);
                            break;
                        case MFInt:
                            valField->Int(record, ((int *) data)[itemID]);
                            break;
                        case MFFloat:
                            valField->Float(record, ((float *) data)[itemID]);
                            break;
                        case MFDouble:
                            valField->Float(record, ((double *) data)[itemID]);
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                        }
                    }
                }
                if (ferror (inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
                delete pntIF;
            }
            break;
        case DBTypeGridContinuous:
        case DBTypeGridDiscrete: {
            DBGridIF *gridIF = new DBGridIF(outData);

            while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
                if (header.ItemNum != gridIF->RowNum() * gridIF->ColNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency!");
                    return (DBFault);
                }
                if (layerID == 0) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = realloc(data, header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    record = gridIF->Layer(layerID);
                    gridIF->RenameLayer(header.Date);
                }
                else record = gridIF->AddLayer(header.Date);

                switch (header.Type) {
                    case MFByte:
                    case MFShort:
                    case MFInt:
                        gridIF->MissingValue(record, header.Missing.Int);
                        break;
                    case MFFloat:
                    case MFDouble:
                        gridIF->MissingValue(record, header.Missing.Float);
                        break;
                    default:
                        CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                }
                if ((int) fread(data, (size_t) itemSize, (size_t) (header.ItemNum), inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    return (DBFault);
                }

                for (pos.Row = 0; pos.Row < gridIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < gridIF->ColNum(); ++pos.Col) {
                        switch (header.Type) {
                            case MFByte:
                                val = (DBFloat) (((char *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFShort:
                                val = (DBFloat) (((short *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFInt:
                                val = (DBFloat) (((int *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFFloat:
                                val = (DBFloat) (((float *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            case MFDouble:
                                val = (DBFloat) (((double *) data)[pos.Row * gridIF->ColNum() + pos.Col]);
                                break;
                            default:
                                CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                                return (DBFault);
                        }
                        gridIF->Value(record, pos, val);
                    }
                layerID++;
                }
            if (ferror (inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
            gridIF->RecalcStats();
            }
            break;
        case DBTypeNetwork: {
            DBInt cellID;
            DBGridIF *gridIF = new DBGridIF(outData);
            DBNetworkIF *netIF = new DBNetworkIF(tmplData);

            while (MFdsHeaderRead(&header, inFile) == CMsucceeded) {
                if (header.ItemNum != netIF->CellNum()) {
                    CMmsgPrint(CMmsgUsrError, "Error: Datastream inconsistency!");
                    return (DBFault);
                }
                if (layerID == 0) {
                    itemSize = MFVarItemSize(header.Type);
                    if ((data = (void *) realloc(data, (size_t) header.ItemNum * itemSize)) == (void *) NULL) {
                        CMmsgPrint(CMmsgSysError, "Memory allocation error in: %s %d", __FILE__, __LINE__);
                        return (DBFault);
                    }
                    record = gridIF->Layer(layerID);
                    gridIF->RenameLayer(header.Date);
                } else record = gridIF->AddLayer(header.Date);
                if ((int) fread(data, itemSize, header.ItemNum, inFile) != header.ItemNum) {
                    CMmsgPrint(CMmsgSysError, "Error: Data stream read in: %s %d", __FILE__, __LINE__);
                    delete netIF;
                    return (DBFault);
                }

                for (pos.Row = 0; pos.Row < gridIF->RowNum(); ++pos.Row)
                    for (pos.Col = 0; pos.Col < gridIF->ColNum(); ++pos.Col)
                        gridIF->Value(record, pos, gridIF->MissingValue());

                for (cellID = 0; cellID < netIF->CellNum(); ++cellID) {
                    pos = netIF->CellPosition(netIF->Cell(cellID));

                    switch (header.Type) {
                        case MFByte:
                            val = (DBFloat) (((char *) data)[cellID]);
                            break;
                        case MFShort:
                            val = (DBFloat) (((short *) data)[cellID]);
                            break;
                        case MFInt:
                            val = (DBFloat) (((int *) data)[cellID]);
                            break;
                        case MFFloat:
                            val = (DBFloat) (((float *) data)[cellID]);
                            break;
                        case MFDouble:
                            val = (DBFloat) (((double *) data)[cellID]);
                            break;
                        default:
                            CMmsgPrint(CMmsgAppError, "Error: Invalid data type in: %s %d", __FILE__, __LINE__);
                            return (DBFault);
                    }
                    gridIF->Value(record, pos, val);
                }
                layerID++;
            }
            if (ferror(inFile) != 0) CMmsgPrint(CMmsgSysError, "Input file reading error in: %s %d", __FILE__, __LINE__);
            gridIF->RecalcStats();
        } break;
        default:
            CMmsgPrint(CMmsgAppError, "Error: Invalid data in: %s %d", __FILE__, __LINE__);
            break;
    }
    return (DBSuccess);
}
