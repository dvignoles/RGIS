/******************************************************************************

GHAAS Database library V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

DBExpARCGrd.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <DB.hpp>
#include <DBif.hpp>

DBInt DBExportARCGridLayer(DBObjData *data, DBObjRecord *layerRec, const char *fileName) {
    FILE *file;
    DBInt ret;

    if ((file = fopen(fileName, "w")) == (FILE *) NULL) {
        CMmsgPrint(CMmsgSysError, "File Opening Error in: %s %d", __FILE__, __LINE__);
        return (DBFault);
    }
    ret = DBExportARCGridLayer(data, layerRec, file);
    fclose(file);
    return (ret);
}

DBInt DBExportARCGridLayer(DBObjData *data, DBObjRecord *layerRec, FILE *file) {
    DBInt row, col;
    DBPosition pos;
    DBGridIF *gridIF = new DBGridIF(data);

    fprintf(file, "ncols         %d\n", gridIF->ColNum());
    fprintf(file, "nrows         %d\n", gridIF->RowNum());
    fprintf(file, "xllcorner     %.4lf\n", (data->Extent()).LowerLeft.X);
    fprintf(file, "yllcorner     %.4lf\n", (data->Extent()).LowerLeft.Y);
    fprintf(file, "cellsize      %.12lf\n", gridIF->CellWidth());
    switch (data->Type()) {
        case DBTypeGridContinuous:
            switch (gridIF->ValueType()) {
                case DBTableFieldFloat:
                    DBFloat floatVal;
                    fprintf(file, "NODATA_value  %f\n", gridIF->MissingValue());
                    for (row = gridIF->RowNum() - 1; row >= 0; row--) {
                        for (col = 0; col < gridIF->ColNum(); col++) {
                            pos.Row = row;
                            pos.Col = col;
                            if (gridIF->Value(layerRec, pos, &floatVal)) fprintf(file, " %.15g", floatVal);
                        else fprintf(file, " %f", gridIF->MissingValue());
                    }
                    fprintf(file, "\n");
                }
                break;
                case DBTableFieldInt:
                    DBInt intVal;
                    fprintf(file, "NODATA_value  %d\n", DBFault);
                    for (row = gridIF->RowNum() - 1; row >= 0; row--) {
                        for (col = 0; col < gridIF->ColNum(); col++) {
                            pos.Row = row;
                            pos.Col = col;
                            fprintf(file, " %d", intVal);
                        }
                        fprintf(file, "\n");
                    }
                break;
            }
        break;
        case DBTypeGridDiscrete:
            fprintf(file, "NODATA_value  %d\n", (DBInt) gridIF->MissingValue ());
            for (row = gridIF->RowNum() - 1; row >= 0; row--) {
                for (col = 0; col < gridIF->ColNum(); col++) {
                    pos.Row = row;
                    pos.Col = col;
                    fprintf(file, " %d", gridIF->GridValue(layerRec, pos));
                }
                fprintf(file, "\n");
            }
        break;
    }
    delete gridIF;
    return (DBSuccess);
}
