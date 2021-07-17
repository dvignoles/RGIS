/******************************************************************************

GHAAS RiverGIS Utilities V3.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

CMDrgisMetadb.cpp

bfekete@gc.cuny.edu

*******************************************************************************/

#include <cm.h>
#include <DB.hpp>
#include <RG.hpp>

static void _CMDprintUsage (const char *arg0) {
    CMmsgPrint(CMmsgInfo, "%s -m [metadb file] <rgis data file> ... <rgis data file>", CMfileName(arg0));
    CMmsgPrint(CMmsgInfo, "     -V,--verbose");
}

int main(int argc, char *argv[]) {
    int i, verbose = false;
    char *ghaasMetaDB = (char *) NULL, metaFileName[DBDataFileNameLen];
    DBObjData *dbData;
    DBDataset *dbDataset;
    DBObjectLIST<DBObjMetaEntry> *metaList;
    DBObjMetaEntry *metaEntry;

    if (argc < 2) {
        _CMDprintUsage(argv[0]);
        return (0);
    }

    for (i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            _CMDprintUsage(argv[0]);
            return (0);
        }
        else if ((strcmp(argv[i], "-V") == 0) || (strcmp(argv[i], "--verbose") == 0)) verbose = true;
        else if (((strcmp(argv[i], "-m") == 0) || (strcmp(argv[i], "--MetaDB") == 0)) && (argc > i + 1)) {
            if (++i < argc) {
                ghaasMetaDB = argv[i];
                break;
            } else {
                _CMDprintUsage(argv[0]);
                return (0);
            }
        }
    }
    if (i == argc) i = 0;
    if (verbose) RGlibPauseOpen(argv[0]);

    if (ghaasMetaDB == (char *) NULL) {
        sprintf(metaFileName, "%s/GHAASMetadb", getenv("GHAAS_DIR") == NULL ? getenv("HOME") : getenv("GHAAS_DIR"));
        ghaasMetaDB = metaFileName;
    }
    dbDataset = new DBDataset((char *) "GHAASmetadb", ghaasMetaDB);
    metaList = dbDataset->MetaList();

    for (; i < argc - 1; ++i) {
        dbData = new DBObjData();
        if (dbData->Read(argv[i + 1]) == DBSuccess) {
            dbData->FileName(argv[i + 1]);
            dbDataset->Data(dbData);
            printf("%s\n", dbData->Name());
            metaEntry = metaList->Item(dbData->Name());
            metaEntry->FileName(dbData->FileName());
            dbDataset->RemoveData(dbData);
        }
        delete dbData;
    }
    delete dbDataset;
    if (verbose) RGlibPauseClose();
    return (0);
}
