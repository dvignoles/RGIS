/******************************************************************************

GHAAS RiverGIS Libarary V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

RGlibTable.C

bfekete@gc.cuny.edu

*******************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <DB.H>
#include <RG.H>

DBInt RGLibTableJoin(DBObjTable *itemTable, DBObjTableField *relateField,
                     DBObjTable *joinTable, DBObjTableField *joinField) {
    DBInt itemID, joinID, fieldID, fieldNum = 0;
    DBObjectLIST<DBObjTableField> *fields = joinTable->Fields();
    DBObjRecord *itemRec, *joinRec;
    DBObjTableField *field, **newFields;

    if ((newFields = (DBObjTableField **) calloc(1, sizeof(DBObjTableField *))) == (DBObjTableField **) NULL) {
        CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: RGLibTableJoin ()");
        return (DBFault);
    }

    if ((newFields[fieldNum] = itemTable->Field(joinTable->Name())) == (DBObjTableField *) NULL) {
        newFields[fieldNum] = new DBObjTableField(joinTable->Name(), DBTableFieldString, "%s", DBStringLength);
        itemTable->AddField(newFields[fieldNum]);
    }
    for (fieldID = 0; fieldID < fields->ItemNum(); fieldID++) {
        field = fields->Item(fieldID);
        if (DBTableFieldIsVisible(field)) {
            fieldNum++;
            newFields = (DBObjTableField **) realloc(newFields, (fieldNum + 1) * sizeof(DBObjTableField *));
            if (newFields == (DBObjTableField **) NULL) {
                CMmsgPrint(CMmsgSysError, "Memory Allocation Error in: RGLibTableJoin ()");
                return (DBFault);
            }
            if ((newFields[fieldNum] = itemTable->Field(field->Name())) == (DBObjTableField *) NULL) {
                itemTable->AddField(newFields[fieldNum] = new DBObjTableField(*field));
                newFields[fieldNum]->Required(false);
            }
        }
    }

    for (itemID = 0; itemID < itemTable->ItemNum(); itemID++) {
        itemRec = itemTable->Item(itemID);
        DBPause(itemID * 100 / itemTable->ItemNum());
        if ((itemRec->Flags() & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
        for (joinID = 0; joinID < joinTable->ItemNum(); joinID++) {
            joinRec = joinTable->Item(joinID);
            if ((joinRec->Flags() & DBObjectFlagIdle) == DBObjectFlagIdle) continue;
            if (DBTableFieldMatch(relateField, itemRec, joinField, joinRec)) {
                fieldNum = 0;
                newFields[fieldNum]->String(itemRec, joinRec->Name());
                for (fieldID = 0; fieldID < fields->ItemNum(); fieldID++) {
                    field = fields->Item(fieldID);
                    if (DBTableFieldIsVisible(field)) {
                        fieldNum++;
                        if (newFields[fieldNum]->Required()) continue;
                        switch (newFields[fieldNum]->Type()) {
                            default:
                            case DBTableFieldString:
                                newFields[fieldNum]->String(itemRec, field->String(joinRec));
                                break;
                            case DBTableFieldInt:
                                newFields[fieldNum]->Int(itemRec, field->Int(joinRec));
                                break;
                            case DBTableFieldFloat:
                                newFields[fieldNum]->Float(itemRec, field->Float(joinRec));
                                break;
                            case DBTableFieldDate:
                                newFields[fieldNum]->Date(itemRec, field->Date(joinRec));
                                break;
                        }
                    }
                }
                break;
            }
        }
    }
    free(newFields);
    return (itemID < itemTable->ItemNum() ? DBFault : DBSuccess);
}

static const char *_RGlibSQLCaseChange (DBInt sqlCase, const char *str, char **bufferPtr, DBInt *bufferLen) {
    DBInt i, len;
    if (sqlCase == RGlibSQLCaseSensitive) return (str);
    len = strlen (str);

    if (len >= *bufferLen) { *bufferPtr = (char *) realloc (*bufferPtr,len + 1);}
    if (*bufferPtr == (char *) NULL) return ("Allocation Error");
    *bufferLen = len;

    for (i = 0;i < len; ++i) (*bufferPtr) [i] = sqlCase == RGlibSQLCaseLower ? tolower (str [i]) : toupper (str [i]);
    (*bufferPtr) [len] = 0;
    return (*bufferPtr);
}

DBInt RGlibTableToSQL (DBObjTable *table, const char *dbSchemaName, const char *dbTableName, RGlibTableAction mode, bool recordName, RGlibSQLdialect dialect, DBInt sqlCase, FILE *outFile) {
    DBObjectLIST<DBObjTableField> *fields = table->Fields();
    DBObjRecord *record;
    DBObjTableField *field;
    DBInt i;
    DBInt bufferLen [2] = {0,0}, recordID, nameLength = 1;
    char *bufferPtr [2] = {(char *) NULL, (char *) NULL}, *notExists, *separator, *encap_begin, *encap_end;
    char schemaName [DBDataFileNameLen], tableName [DBDataFileNameLen];

    notExists = RGlibTableCopy == mode ? (char *) " IF NOT EXISTS " : (char *) " ";
    for (i = strlen (dbTableName) - 1; i >= 0; --i) tableName [i] = ((dbTableName [i] == '-') || (dbTableName [i] == '+')) ? '_' : dbTableName [i];
    if (dbSchemaName == (char *) NULL) {
        fprintf (outFile, "-- Table: \"%s\"\n",             _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen));
        if (RGlibTableCopy != mode) {
            fprintf (outFile, "DROP TABLE IF EXISTS \"%s\";\n", _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen));
        }
        fprintf (outFile, "CREATE TABLE%s\"%s\"\n",         notExists, _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen));
    }
    else {
        for (i = strlen (dbSchemaName) - 1; i >= 0; --i) schemaName [i] = ((dbSchemaName [i] == '-') || (dbSchemaName [i] == '+')) ? '_' : dbSchemaName [i];
        switch (dialect)
        {
            case RGlibSQLpostgres:
                fprintf (outFile, "-- Table: \"%s\".\"%s\"\n",                  _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                if (RGlibTableCopy != mode) {
                    fprintf (outFile, "DROP TABLE IF EXISTS \"%s\".\"%s\";\n",  _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                }
                fprintf (outFile, "CREATE TABLE%s\"%s\".\"%s\" (\n", notExists, _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                encap_begin = encap_end = (char *) "$$";
                break;
            case RGlibSQLite:
                fprintf (outFile, "-- Table: \"%s_%s\"\n",                  _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                if (RGlibTableCopy != mode) {
                    fprintf (outFile, "DROP TABLE IF EXISTS \"%s_%s\";\n",  _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                }
                fprintf (outFile, "CREATE TABLE%s\"%s_%s\" (\n", notExists, _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen), _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                encap_begin = (char *) "\"";
                encap_end   = (char *) "\"";
                break;
        }
    }
    for (recordID = 0;recordID < table->ItemNum();++recordID) {
        record = table->Item (recordID);
        nameLength = nameLength > strlen (record->Name ()) ? nameLength : strlen (record->Name ());
    }

    switch (dialect)
    {
        case RGlibSQLpostgres:
            fprintf (outFile,"\"%s\" SERIAL",_RGlibSQLCaseChange (sqlCase, "ID", bufferPtr, bufferLen));
            if (recordName) fprintf (outFile,",\n\"%s\" CHARACTER VARYING(%d) COLLATE pg_catalog.\"default\"",_RGlibSQLCaseChange (sqlCase, "RecordName", bufferPtr, bufferLen),nameLength + 1);
            for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
                if (DBTableFieldIsVisible (field))
                    switch (field->Type()) {
                        default:
                        case DBTableFieldString:
                            fprintf(outFile, ",\n\"%s\" CHARACTER VARYING(%d) COLLATE pg_catalog.\"default\"", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen),
                                    field->Length());
                            break;
                        case DBTableFieldInt:
                            fprintf(outFile, ",\n\"%s\" INTEGER", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                        case DBTableFieldFloat:
                            fprintf(outFile, ",\n\"%s\" FLOAT",   _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                        case DBTableFieldDate:
                            fprintf(outFile, ",\n\"%s\" CHARACTER VARYING(10) COLLATE pg_catalog.\"default\"", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                    }
            }
            fprintf (outFile,",\nCONSTRAINT \"%s:pkey\" PRMARY KEY (\"%s\")", _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen),_RGlibSQLCaseChange (sqlCase, "ID", bufferPtr + 1, bufferLen + 1));
            fprintf (outFile,") WITH ( OIDS = FALSE ) TABLESPACE pg_default;\n");
            break;
        case RGlibSQLite:
            fprintf (outFile,"\"%s\" INTEGER PRIMARY KEY AUTOINCREMENT",_RGlibSQLCaseChange (sqlCase, "ID", bufferPtr, bufferLen));
            if (recordName) fprintf (outFile,",\n\"%s\" TEXT", _RGlibSQLCaseChange (sqlCase, "RecordName", bufferPtr, bufferLen));
            for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
                if (DBTableFieldIsVisible (field))
                    switch (field->Type()) {
                        default:
                        case DBTableFieldString:
                            fprintf(outFile, ",\n\"%s\" TEXT",    _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                        case DBTableFieldInt:
                            fprintf(outFile, ",\n\"%s\" INTEGER", _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                        case DBTableFieldFloat:
                            fprintf(outFile, ",\n\"%s\" FLOAT",   _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                        case DBTableFieldDate:
                            fprintf(outFile, ",\"%s\n\" TEXT",    _RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                            break;
                    }
            }
            fprintf (outFile,");\n");
            break;
    }

    if ((mode != RGlibTableBlank)) {
        if (mode == RGlibTableCopy) {
            switch (dialect)
            {
                case RGlibSQLpostgres:
                    if (dbSchemaName == (char *) NULL) fprintf (outFile,"\nTRUNCATE TABLE \"%s\"          RESTART IDENTITY CASCADE;", _RGlibSQLCaseChange (sqlCase, tableName,  bufferPtr, bufferLen));
                    else                               fprintf (outFile,"\nTRUNCATE TABLE \"%s\".\"%s\"   RESTART IDENTITY CASCADE;", _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen),  _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                    break;
                case RGlibSQLite:
                    if (dbSchemaName == (char *) NULL) fprintf (outFile,"\nDELETE FROM \"%s\";",    _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen));
                    else                               fprintf (outFile,"\nDELETE FROM \"%s_%s\";", _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen),  _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                    break;
            }
        }
        if (dbSchemaName == (char *) NULL) fprintf (outFile,"\nINSERT INTO  \"%s\" (",  _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr, bufferLen));
        else
            switch (dialect)
            {
                case RGlibSQLpostgres:
                    fprintf (outFile,"\nINSERT INTO  \"%s\".\"%s\" (", _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen),  _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                    break;
                case RGlibSQLite:
                    fprintf (outFile,"\nINSERT INTO  \"%s_%s\" (",     _RGlibSQLCaseChange (sqlCase, schemaName, bufferPtr, bufferLen),  _RGlibSQLCaseChange (sqlCase, tableName, bufferPtr + 1, bufferLen + 1));
                    break;
            }
        if (recordName) {
            fprintf (outFile,"\"%s\"",_RGlibSQLCaseChange (sqlCase, "RecordName", bufferPtr, bufferLen));
            separator = (char *) ", ";
        }
        else separator = (char *) "";

        for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
            if (DBTableFieldIsVisible (field)) {
                fprintf (outFile,"%s\"%s\"",separator,_RGlibSQLCaseChange (sqlCase, field->Name(), bufferPtr, bufferLen));
                separator = (char *) ", ";
            }
        }
        fprintf (outFile,") VALUES\n");
        for (record = table->First (); record != (DBObjRecord *) NULL; record = table->Next ()) {
            if (record->RowID () == 0) {
                if (recordName) { fprintf (outFile,    "(%s%s%s", encap_begin, record->Name(), encap_end); separator = (char *) ", "; }
                else            { fprintf (outFile,    "(");                                               separator = (char *) ""; }
            }
            else {
                if (recordName) { fprintf (outFile,"),\n(%s%s%s", encap_begin, record->Name(), encap_end);  separator = (char *) ", "; }
                else            { fprintf (outFile,"),\n(");                                                separator = (char *) ""; }
            }
            for (field = fields->First(); field != (DBObjTableField *) NULL; field = fields->Next()) {
                if (DBTableFieldIsVisible (field)) {
                    switch (field->Type()) {
                        default:
                        case DBTableFieldString:
                            fprintf(outFile, "%s%s%s%s", separator, encap_begin, field->String(record), encap_end);
                            break;
                        case DBTableFieldInt:
                            fprintf(outFile, "%s%d",     separator, field->Int(record));
                            break;
                        case DBTableFieldFloat:
                            fprintf(outFile, "%s%f",     separator, field->Float(record));
                            break;
                        case DBTableFieldDate:
                            fprintf(outFile, "%s\'%s\'", separator, field->String(record));
                            break;
                    }
                    separator = (char *) ", ";
                }
            }
        }
        fprintf (outFile,");\n");
    }
    if (bufferPtr [0] != (char *) NULL) free (bufferPtr [0]);
    if (bufferPtr [1] != (char *) NULL) free (bufferPtr [1]);
    return (CMsucceeded);
 }
