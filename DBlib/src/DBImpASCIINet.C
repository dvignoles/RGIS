/******************************************************************************

GHAAS Database library V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

DBImpASCIINet.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <ctype.h>
#include <DB.H>
#include <DBif.H>

#define NCOLS     "ncols"
#define NROWS     "nrows"
#define XLLCORNER "xllcorner"
#define YLLCORNER "yllcorner"
#define XLLCENTER "xllcenter"
#define YLLCENTER "yllcenter"
#define CELLSIZE  "cellsize"
#define DX        "dx"
#define DY        "dy"
#define NODATA    "nodata_value"
#define CORNER 0
#define CENTER 1

int DBImportASCIINet(DBObjData *netData, const char *fileName) {
    FILE *file;
    char buffer[81];
    char nameSTR[DBStringLength];
    DBInt i, j, rowNum, colNum, cornerType, noData, gridVal;
    DBFloat cellWidth, cellHeight;
    DBPosition pos;
    DBCoordinate coord;
    DBRegion extent;
    DBObjTable *basinTable = netData->Table(DBrNItems);
    DBObjTable *cellTable = netData->Table(DBrNCells);
    DBObjTable *layerTable = netData->Table(DBrNLayers);

    DBObjTableField *mouthPosFLD = basinTable->Field(DBrNMouthPos);
    DBObjTableField *colorFLD = basinTable->Field(DBrNColor);

    DBObjTableField *positionFLD = cellTable->Field(DBrNPosition);
    DBObjTableField *toCellFLD = cellTable->Field(DBrNToCell);
    DBObjTableField *fromCellFLD = cellTable->Field(DBrNFromCell);
    DBObjTableField *orderFLD = cellTable->Field(DBrNOrder);
    DBObjTableField *basinFLD = cellTable->Field(DBrNBasin);
    DBObjTableField *basinCellsFLD = cellTable->Field(DBrNBasinCells);
    DBObjTableField *travelFLD = cellTable->Field(DBrNTravel);
    DBObjTableField *upCellPosFLD = cellTable->Field(DBrNUpCellPos);
    DBObjTableField *cellAreaFLD = cellTable->Field(DBrNCellArea);
    DBObjTableField *subbasinLengthFLD = cellTable->Field(DBrNSubbasinLength);
    DBObjTableField *subbasinAreaFLD = cellTable->Field(DBrNSubbasinArea);

    DBObjTableField *rowNumFLD = layerTable->Field(DBrNRowNum);
    DBObjTableField *colNumFLD = layerTable->Field(DBrNColNum);
    DBObjTableField *cellWidthFLD = layerTable->Field(DBrNCellWidth);
    DBObjTableField *cellHeightFLD = layerTable->Field(DBrNCellHeight);
    DBObjTableField *valueTypeFLD = layerTable->Field(DBrNValueType);
    DBObjTableField *valueSizeFLD = layerTable->Field(DBrNValueSize);
    DBObjTableField *layerFLD = layerTable->Field(DBrNLayer);

    DBObjRecord *layerRec, *dataRec, *cellRec, *basinRec;
    DBNetworkIF *netIF;

    if ((file = fopen(fileName, "r")) == NULL) {
        CMmsgPrint(CMmsgSysError, "File Opening Error in: %s %d", __FILE__, __LINE__);
        return (DBFault);
    }

    for (i = 0; i < 6; ++i)
        if (fgets(buffer, sizeof(buffer) - 1, file) == (char *) NULL) {
            CMmsgPrint(CMmsgSysError, "File Reading Error in: %s %d", __FILE__, __LINE__);
            return (DBFault);
        }
        else {
            for (j = 0; (j < (int) strlen(buffer)) && (buffer[j] != ' '); ++j)
                buffer[j] = tolower(buffer[j]);
            if (strncmp(buffer, NCOLS, strlen(NCOLS)) == 0)
                sscanf(buffer + strlen(NCOLS), "%d", &colNum);
            if (strncmp(buffer, NROWS, strlen(NROWS)) == 0)
                sscanf(buffer + strlen(NROWS), "%d", &rowNum);
            if (strncmp(buffer, XLLCORNER, strlen(XLLCORNER)) == 0) {
                sscanf(buffer + strlen(XLLCORNER), "%lf", &(coord.X));
                cornerType = CORNER;
            }
            if (strncmp(buffer, YLLCORNER, strlen(YLLCORNER)) == 0) {
                sscanf(buffer + strlen(YLLCORNER), "%lf", &(coord.Y));
                cornerType = CORNER;
            }
            if (strncmp(buffer, XLLCENTER, strlen(XLLCENTER)) == 0) {
                sscanf(buffer + strlen(XLLCENTER), "%lf", &(coord.X));
                cornerType = CENTER;
            }
            if (strncmp(buffer, YLLCENTER, strlen(YLLCENTER)) == 0) {
                sscanf(buffer + strlen(YLLCENTER), "%lf", &(coord.Y));
                cornerType = CENTER;
            }
            if (strncmp(buffer, DX, strlen(DX)) == 0)
                sscanf(buffer + strlen(DX), "%lf", &cellWidth);
            if (strncmp(buffer, DY, strlen(DY)) == 0)
                sscanf(buffer + strlen(DY), "%lf", &cellHeight);
            if (strncmp(buffer, CELLSIZE, strlen(CELLSIZE)) == 0)
            { sscanf(buffer + strlen(CELLSIZE), "%lf", &cellWidth); cellHeight = cellWidth; }
            if (strncmp(buffer, NODATA, strlen(NODATA)) == 0)
                sscanf(buffer + strlen(NODATA), "%d", &noData);
        }
    layerTable->Add(DBrNLookupGrid);
    if ((layerRec = layerTable->Item(DBrNLookupGrid)) == (DBObjRecord *) NULL) {
        CMmsgPrint(CMmsgAppError, "Network Layer Creation Error in: %s %d", __FILE__, __LINE__);
        return (DBFault);
    }

    cellWidthFLD->Float(layerRec, cellWidth);
    cellHeightFLD->Float(layerRec, cellHeight);
    valueTypeFLD->Int(layerRec, DBTableFieldInt);
    valueSizeFLD->Int(layerRec, sizeof(DBInt));
    rowNumFLD->Int(layerRec, rowNum);
    colNumFLD->Int(layerRec, colNum);
    dataRec = new DBObjRecord("NetLookupGridRecord", (size_t) rowNum * (size_t) colNum, sizeof(DBInt));
    if (dataRec->Data () == (void *) NULL) { delete dataRec; return (DBFault); }

    layerFLD->Record(layerRec, dataRec);
    (netData->Arrays())->Add(dataRec);
    for (pos.Row = rowNum - 1; pos.Row >= 0; --pos.Row)
        for (pos.Col = 0; pos.Col < colNum; ++pos.Col) {
            if (fscanf(file, "%d", &gridVal) != 1) {
                CMmsgPrint(CMmsgUsrError, "Warning: Incomplete network grid!");
                goto Stop;
            }
            else {
                if (gridVal == noData)
                    ((DBInt *) dataRec->Data())[(size_t) pos.Row * (size_t) colNum + (size_t) pos.Col] = DBFault;
                else {
                    sprintf(nameSTR, "GHAASCell:%d", cellTable->ItemNum());
                    cellRec = cellTable->Add(nameSTR);
                    positionFLD->Position(cellRec, pos);
                    toCellFLD->Int(cellRec, gridVal);
                    fromCellFLD->Int(cellRec, (DBInt) 0);
                    orderFLD->Int(cellRec, (DBInt) 0);
                    basinFLD->Int(cellRec, (DBInt) 0);
                    basinCellsFLD->Int(cellRec, (DBInt) 0);
                    travelFLD->Int(cellRec, (DBInt) 0);
                    upCellPosFLD->Position(cellRec, pos);
                    cellAreaFLD->Float(cellRec, (DBFloat) 0.0);
                    subbasinLengthFLD->Float(cellRec, (DBFloat) 0.0);
                    subbasinAreaFLD->Float(cellRec, (DBFloat) 0.0);

                    ((DBInt *) dataRec->Data())[(size_t) pos.Row * (size_t) colNum + (size_t) pos.Col] = cellRec->RowID();
                }
            }
        }
Stop:
    sprintf(nameSTR, "GHAASBasin%d", (DBInt) 0);
    basinRec = basinTable->Add(nameSTR);
    mouthPosFLD->Position(basinRec, positionFLD->Position(cellTable->Item(0)));
    colorFLD->Int(basinRec, 0);
    netData->Precision(cellWidth / 25.0);
    coord.X = coord.X - cornerType * cellWidth / 2.0;
    coord.Y = coord.Y - cornerType * cellHeight / 2.0;
    extent.Expand(coord);
    coord.X += colNum * cellWidth;
    coord.Y += rowNum * cellHeight;
    extent.Expand(coord);
    netData->Extent(extent);
    netData->Projection(DBMathGuessProjection(extent));
    netData->Precision(DBMathGuessPrecision(extent));
    netIF = new DBNetworkIF(netData);
    netIF->Build();
    delete netIF;
    fclose(file);
    return (DBSuccess);
}
